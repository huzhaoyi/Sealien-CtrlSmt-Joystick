/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-09-08 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-09-08 11:29:37
 * @FilePath: src/core/application_controller.cpp
 * @Description: 应用程序主控制器实现文件
 * 负责协调各个模块，管理应用程序的生命周期
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "application_controller.h"
#include "config/smart_config.h"
#include "constants.h"
#include "utils.h"
#include "utils/debug_config.h"

#include <yaml-cpp/yaml.h>
#include <csignal>
#include <algorithm>
#include <atomic>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <limits.h>
#include <linux/limits.h>
#include <set>
#include <vector>

// 全局运行标志（原子），用于信号处理与多线程安全退出
std::atomic<bool> ApplicationController::g_run{true};

ApplicationController::ApplicationController() {
}

ApplicationController::~ApplicationController() {
    stop();
    cleanup();
}

bool ApplicationController::initialize(const std::string& config_path, bool enable_ros2) {
    try {
        // 保存配置文件路径（用于解析相对路径的阀控板配置文件）
        config_path_ = config_path;
        enable_ros2_ = enable_ros2;

        // 注意：调试配置文件已在main()函数中加载，这里不需要重复加载
        
        // 初始化智能配置管理器
        if (!initializeConfig(config_path)) {
            DEBUG_CONFIG_ERROR("配置初始化失败");
            return false;
        }

        // 根据配置文件中的enable_modbus设置决定是否初始化Modbus
        enable_modbus_ = smart_config_->getConfig().getEnableModbus();
        if (enable_modbus_) {
            DEBUG_CORE_LOG("配置已启用 Modbus 功能，正在尝试初始化 Modbus 客户端");
            if (!initializeModbus()) {
                DEBUG_MODBUS_WARNING("Modbus 初始化失败，将以仅 USB 摇杆模式运行");
                enable_modbus_ = false;  // 如果初始化失败，禁用modbus功能
            }
        } else {
            DEBUG_CORE_LOG("Modbus 功能已禁用，跳过 Modbus 初始化");
        }

        // 总是尝试初始化ROS2（自动检测模式）
        if (!initializeROS2(0, nullptr)) {
            DEBUG_CORE_WARNING("ROS2 初始化失败，继续运行但不启用 ROS2");
            enable_ros2_ = false;
        }

        // 初始化手柄管理器（总是尝试，支持动态检测）
        if (!initializeJoystickManager()) {
            DEBUG_CORE_ERROR("摇杆管理器初始化失败");
            return false;
        }

        // 初始化USB转串口设备检测器
        if (!initializeUSBSerialDetector()) {
            DEBUG_CORE_ERROR("USB 串口检测器初始化失败");
            return false;
        }

        // 初始化uinput设备（仅在Modbus模式下需要，但可选）
        if (enable_modbus_ && modbus_connected_) {
            if (!initializeUInput()) {
                DEBUG_UINPUT_ERROR("uinput 初始化失败");
#ifdef ENABLE_ROS2
                // 如果启用了ROS2，即使uinput失败也可以继续运行（仅ROS2输出）
                if (enable_ros2_) {
                    DEBUG_CORE_LOG("uinput不可用，将仅使用ROS2输出模式");
                } else {
                    DEBUG_CORE_ERROR("uinput不可用且未启用ROS2，无法继续运行");
                    DEBUG_CORE_ERROR("提示: Tegra系统可能不支持uinput，请启用ROS2功能或重新编译内核启用uinput支持");
                    return false;
                }
#else
                // 未启用ROS2且uinput失败，无法继续
                DEBUG_CORE_ERROR("uinput不可用且未启用ROS2，无法继续运行");
                DEBUG_CORE_ERROR("提示: Tegra系统可能不支持uinput，请启用ROS2功能或重新编译内核启用uinput支持");
                return false;
#endif
            }

            // 初始化采集卡数据处理器（即使uinput失败也可以继续，如果启用了ROS2）
            if (!initializeAcquisitionCardProcessor()) {
                DEBUG_CORE_ERROR("采集卡数据处理器初始化失败");
                return false;
            }

            // 初始化阀控板处理器（如果启用）
            const auto& config = smart_config_->getConfig();
            if (config.getEnableValveControl()) {
                if (!initializeValveControlProcessor()) {
                    DEBUG_CORE_WARNING("阀控板处理器初始化失败，继续运行但不启用阀控功能");
                }
            } else {
                DEBUG_CORE_LOG("配置中已禁用阀控功能，跳过初始化");
            }
        }

        // 注册信号处理器
        registerSignalHandlers();

        initialized_ = true;
        DEBUG_CORE_LOG("应用程序控制器初始化成功");
        DEBUG_CORE_LOG("Modbus 已启用: " << (enable_modbus_ ? "是" : "否"));
        DEBUG_CORE_LOG("Modbus 已连接: " << (modbus_connected_ ? "是" : "否"));
        return true;

    } catch (const std::exception& e) {
        DEBUG_CORE_LOG("应用程序控制器初始化失败: " << e.what());
        return false;
    }
}

bool ApplicationController::run() {
    if (!initialized_) {
        DEBUG_CORE_LOG("应用程序控制器未初始化");
        return false;
    }

    DEBUG_CORE_LOG("正在启动应用程序主循环...");

#ifdef ENABLE_ROS2
    // 在启动时立即创建并发布一次heartbeat，帮助ROS2发现机制
    if (ros2_initialized_ && ros2_node_) {
        publishSystemHeartbeat();
        // 多次spin确保发布者被宣布
        for (int i = 0; i < 10; ++i) {
            rclcpp::spin_some(ros2_node_);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
#endif

    // 使用动态检测模式
    runDynamicMode();

    DEBUG_CORE_LOG("应用程序主循环已结束");
    return true;
}

void ApplicationController::stop() {
    g_run.store(false, std::memory_order_relaxed);
}

bool ApplicationController::initializeROS2(int argc, char** argv) {
#ifdef ENABLE_ROS2
    try {
        // 初始化ROS2
        rclcpp::init(argc, argv);
        DEBUG_CORE_LOG("ROS2 已初始化");

        // 创建ROS2节点
        ros2_node_ = std::make_shared<rclcpp::Node>(ROS2_NODE_NAME);
        DEBUG_CORE_LOG("ROS2 节点已创建: " << ROS2_NODE_NAME);

        ros2_initialized_ = true;
        return true;
    } catch (const std::exception& e) {
        DEBUG_CORE_LOG("ROS2 初始化失败: " << e.what());
        return false;
    }
#else
    DEBUG_CORE_LOG("此构建中未启用 ROS2");
    return false;
#endif
}

bool ApplicationController::initializeConfig(const std::string& config_path) {
    try {
        // 创建智能配置管理器
        smart_config_ = std::make_unique<SmartConfigManager>();
        
        if (config_path.empty()) {
            // 没有配置文件，使用默认配置
            DEBUG_CONFIG_LOG("未指定配置文件，使用默认配置");
            // 创建一个默认配置
            Config default_config;
            default_config.enable_modbus = false;  // 默认禁用Modbus
            default_config.serial_port = "/dev/ttyUSB0";  // 默认串口
            default_config.baud = 115200;
            default_config.parity = 'N';
            default_config.data_bits = 8;
            default_config.stop_bits = 1;
            default_config.slave_id = 1;
            default_config.poll_hz = 50.0;
            default_config.bus_error_threshold = 2;
            default_config.min_voltage = 0.5;
            default_config.mid_voltage = 2.5;
            default_config.max_voltage = 4.5;
            
            // 手动设置配置（不使用智能检测）
            if (!smart_config_->setConfig(default_config)) {
                DEBUG_CONFIG_ERROR("无法设置默认配置");
                return false;
            }
            smart_config_->enableSmartDetection(false);
            
            poll_hz_ = 50.0;     // 默认轮询频率
            return true;
        } else {
            // 加载指定配置文件，启用智能串口检测
            if (!smart_config_->loadConfig(config_path, true)) {
                DEBUG_CONFIG_ERROR("无法从以下路径加载配置: " << config_path);
                return false;
            }
            
            const auto& config = smart_config_->getConfig();
            DEBUG_CONFIG_LOG("配置已加载. serial=" << config.getSerialPort() 
                             << " baud=" << config.getBaud() 
                             << " slave=" << config.getSlaveId());
            DEBUG_CONFIG_LOG("智能串口检测: " << (smart_config_->isSmartDetectionEnabled() ? "已启用" : "已禁用"));
            DEBUG_CONFIG_LOG("串口状态: " << smart_config_->getSerialPortStatus());

            // 计算轮询周期
            poll_hz_ = std::max(1.0, config.getPollHz());
            
            // 设置串口变化回调
            smart_config_->setSerialPortCallback([this](const std::string& old_port, const std::string& new_port) {
                DEBUG_CORE_LOG("串口已更改: " << old_port << " -> " << new_port);
                // 如果Modbus客户端存在，需要重新初始化
                if (modbus_client_) {
                    DEBUG_CORE_LOG("正在使用新端口重新初始化 Modbus 客户端");
                    initializeModbus();
                }
            });
            
            return true;
        }
    } catch (const std::exception& e) {
        DEBUG_CONFIG_LOG("加载配置失败: " << e.what());
        return false;
    }
}

bool ApplicationController::initializeModbus() {
    try {
        const auto& config = smart_config_->getConfig();
        
        // 优先使用配置的序列号查找设备
        std::string modbus_port = config.getSerialPort();
        if (!config.getSerialNumber().empty()) {
            // 配置了序列号，通过序列号查找设备
            std::string found_port = findDeviceBySerialNumber(config.getSerialNumber());
            if (!found_port.empty()) {
                modbus_port = found_port;
                DEBUG_CORE_LOG("通过配置的序列号找到采集卡: " << config.getSerialNumber() << " -> " << modbus_port);
                // 更新配置中的端口
                smart_config_->setSerialPort(modbus_port);
            } else {
                DEBUG_CORE_WARNING("配置的序列号 " << config.getSerialNumber() << " 未找到，使用配置的端口: " << modbus_port);
            }
        }
        
        modbus_client_ = std::make_unique<ModbusClient>(
            modbus_port, 
            config.getBaud(), 
            config.getParity(), 
            config.getDataBits(), 
            config.getStopBits(), 
            config.getSlaveId(), 
            config.getBusErrorThreshold(),
            "采集卡"  // 设备类型为"采集卡"
        );

        // 启用自动重连功能
        modbus_client_->enableAutoReconnect(true, MODBUS_AUTO_RECONNECT_INTERVAL_MS, MODBUS_AUTO_RECONNECT_MAX_ATTEMPTS);
        
        // 设置连接状态变化回调
        modbus_client_->setConnectionCallback([this](const std::string& /* device_path */, bool connected) {
            modbus_connected_ = connected;
        });

        modbus_connected_ = modbus_client_->connect();
        if (modbus_connected_) {
            DEBUG_MODBUS_COMM_LOG("Modbus 已连接");
            // 保存采集卡的 USB 序列号（用于重连识别）
            // 优先使用配置的序列号，如果没有配置则从设备读取
            if (!config.getSerialNumber().empty()) {
                acquisition_card_serial_number_ = config.getSerialNumber();
                DEBUG_CORE_LOG("采集卡序列号 (来自配置): " << acquisition_card_serial_number_);
            } else {
                std::string current_port = smart_config_->getCurrentSerialPort();
                acquisition_card_serial_number_ = getUSBSerialNumber(current_port);
                if (!acquisition_card_serial_number_.empty()) {
                    DEBUG_CORE_LOG("采集卡序列号 (来自设备): " << acquisition_card_serial_number_);
                }
            }
        } else {
            DEBUG_MODBUS_COMM_LOG("Modbus 初始连接失败，已启用自动重连");
        }
        return true;
    } catch (const std::exception& e) {
        DEBUG_MODBUS_COMM_LOG("Modbus 初始化失败: " << e.what());
        return false;
    }
}

bool ApplicationController::initializeUInput() {
    try {
        uinput_device_ = std::make_unique<UInputDevice>();

        const auto& config = smart_config_->getConfig();
        
        // 准备uinput设备：收集所有需要注册的轴和按键代码
        std::vector<int> abs_codes;
        for (auto& a : config.getAxes()) {
            abs_codes.push_back(a.code);           // 连续轴
        }
        for (auto& da : config.getDiscreteAxes()) {
            abs_codes.push_back(da.code);          // 离散轴
        }

        std::vector<int> key_codes;
        for (auto& b : config.getButtons()) {
            key_codes.push_back(b.code);           // 按钮
        }

        // 创建虚拟输入设备
        if (!uinput_device_->create("Modbus Virtual Joystick (cfg)", 0x23A7, 0x0002, 1, abs_codes, key_codes)) {
            DEBUG_UINPUT_LOG("uinput 创建失败");
            return false;
        }

        // 设置所有轴的取值范围为[AXIS_MIN_VALUE, AXIS_MAX_VALUE]
        for (int code : abs_codes) {
            uinput_device_->setAbsRange(code, AXIS_MIN_VALUE, AXIS_MAX_VALUE, 0, 0, 0);
        }

        uinput_created_ = true;
        DEBUG_UINPUT_LOG("uinput 摇杆已创建");
        
        return true;
    } catch (const std::exception& e) {
        DEBUG_UINPUT_LOG("uinput 初始化失败: " << e.what());
        return false;
    }
}

bool ApplicationController::initializeAcquisitionCardProcessor() {
    try {
        const auto& config = smart_config_->getConfig();
        // 如果uinput初始化失败，传入nullptr（仅ROS2模式）
        UInputDevice* uinput_ptr = uinput_created_ && uinput_device_ ? uinput_device_.get() : nullptr;
        
#ifdef ENABLE_ROS2
        acquisition_card_processor_ = std::make_unique<AcquisitionCardProcessor>(
            const_cast<Config&>(config), 
            *modbus_client_, 
            uinput_ptr,
            ros2_node_
        );
#else
        acquisition_card_processor_ = std::make_unique<AcquisitionCardProcessor>(
            const_cast<Config&>(config), 
            *modbus_client_, 
            uinput_ptr
        );
#endif

        if (!acquisition_card_processor_->initialize()) {
            DEBUG_CORE_LOG("采集卡数据处理器初始化失败");
            return false;
        }
        
        // 读取轴映射参数并设置
#ifdef ENABLE_ROS2
        if (enable_ros2_ && ros2_node_) {
            int linear_x = ros2_node_->get_parameter("axis_mapping.linear_x").as_int();
            int linear_y = ros2_node_->get_parameter("axis_mapping.linear_y").as_int();
            int linear_z = ros2_node_->get_parameter("axis_mapping.linear_z").as_int();
            int angular_x = ros2_node_->get_parameter("axis_mapping.angular_x").as_int();
            int angular_y = ros2_node_->get_parameter("axis_mapping.angular_y").as_int();
            int angular_z = ros2_node_->get_parameter("axis_mapping.angular_z").as_int();
            
            acquisition_card_processor_->setAxisMapping(linear_x, linear_y, linear_z, angular_x, angular_y, angular_z);
            DEBUG_CORE_LOG("采集卡轴映射已从ROS2参数配置");
        }
#endif

        DEBUG_CORE_LOG("采集卡数据处理器初始化成功");
        return true;
    } catch (const std::exception& e) {
        DEBUG_CORE_LOG("采集卡数据处理器初始化失败: " << e.what());
        return false;
    }
}

bool ApplicationController::initializeValveControlProcessor() {
    try {
        const auto& config = smart_config_->getConfig();
        
        // 检查是否启用阀控板功能
        if (!config.getEnableValveControl()) {
            DEBUG_CORE_LOG("配置中已禁用阀控功能");
            return true; // 不启用阀控板不算错误
        }
        
        // 如果指定了独立的阀控板配置文件，则加载它
        Config valve_config = config;
        std::string valve_config_file = config.getValveControlConfigFile();
        if (!valve_config_file.empty()) {
            // 处理相对路径：如果路径不是绝对路径，则相对于项目根目录
            std::string full_path = valve_config_file;
            if (valve_config_file[0] != '/') {
                // 相对路径，需要基于项目根目录
                // 程序在 build 目录运行，所以需要加上 ../ 前缀
                // 如果路径以 config/ 开头，说明是从项目根目录开始的
                if (valve_config_file.find("config/") == 0) {
                    full_path = "../" + valve_config_file;
                } else {
                    // 其他相对路径，也加上 ../ 前缀（假设相对于项目根目录）
                    full_path = "../" + valve_config_file;
                }
            }
            
            DEBUG_CORE_LOG("正在从以下路径加载阀控板配置: " << full_path << " (原始路径: " << valve_config_file << ")");
            try {
                // 加载阀控板配置文件，只提取 valve_control 段
                YAML::Node y = YAML::LoadFile(full_path);
                if (y["valve_control"]) {
                    auto vc = y["valve_control"];
                    if (vc["serial_port"]) {
                        std::string serial_port = vc["serial_port"].as<std::string>();
                        if (serial_port == "auto" || serial_port == "smart") {
                            // 使用智能串口检测
                            if (smart_config_->isSmartDetectionEnabled()) {
                                // 获取采集卡使用的串口，避免冲突
                                std::string acquisition_card_port = smart_config_->getCurrentSerialPort();
                                
                                // 获取所有检测到的设备
                                auto detected_devices = smart_config_->getDetectedDevices();
                                
                                // 选择最佳设备（排除采集卡使用的串口）
                                std::string best_port = "";
                                int best_priority = -1;
                                
                                for (const auto& device : detected_devices) {
                                    // 跳过采集卡使用的串口
                                    if (device.device_path == acquisition_card_port) {
                                        DEBUG_CORE_LOG("跳过采集卡端口用于阀控板: " << device.device_path);
                                        continue;
                                    }
                                    
                                    // 选择优先级最高的设备
                                    if (device.priority > best_priority) {
                                        best_priority = device.priority;
                                        best_port = device.device_path;
                                    }
                                }
                                
                                if (!best_port.empty()) {
                                    valve_config.valve_control_port = best_port;
                                    DEBUG_CORE_LOG("阀控板端口通过智能检测选择: " << best_port 
                                                  << " (优先级: " << best_priority << ", 采集卡端口: " << acquisition_card_port << ")");
                                } else {
                                    // 如果没有找到其他设备，尝试使用智能检测的默认选择
                                    std::string detected_port = smart_config_->selectBestDevice("");
                                    if (!detected_port.empty() && detected_port != acquisition_card_port) {
                                        valve_config.valve_control_port = detected_port;
                                        DEBUG_CORE_LOG("阀控板端口通过智能检测选择 (备用): " << detected_port);
                                    } else {
                            valve_config.valve_control_port = DEFAULT_SERIAL_PORT;
                                        DEBUG_CORE_WARNING("智能检测未找到合适的阀控板端口 (采集卡使用: " 
                                                          << acquisition_card_port << "), 使用默认: " << DEFAULT_SERIAL_PORT);
                                    }
                                }
                        } else {
                                // 智能检测未启用，使用默认值
                                valve_config.valve_control_port = DEFAULT_SERIAL_PORT;
                                DEBUG_CORE_WARNING("智能检测已禁用，使用阀控板默认端口: " << DEFAULT_SERIAL_PORT);
                            }
                        } else {
                            // 指定了具体串口路径，直接使用
                            valve_config.valve_control_port = serial_port;
                        }
                    }
                    // 解析序列号（如果配置了）
                    if (vc["serial_number"]) {
                        valve_config.valve_control_serial_number = vc["serial_number"].as<std::string>();
                    }
                    if (vc["baud"]) valve_config.valve_control_baud = vc["baud"].as<int>();
                    if (vc["parity"]) {
                        std::string parity_str = vc["parity"].as<std::string>();
                        if (parity_str.length() > 0) {
                            valve_config.valve_control_parity = parity_str[0];
                        }
                    }
                    if (vc["data_bits"]) valve_config.valve_control_data_bits = vc["data_bits"].as<int>();
                    if (vc["stop_bits"]) valve_config.valve_control_stop_bits = vc["stop_bits"].as<int>();
                    if (vc["slave_id"]) valve_config.valve_control_slave_id = vc["slave_id"].as<int>();
                    DEBUG_CORE_LOG("已从独立文件加载阀控板配置");
                } else {
                    DEBUG_CORE_WARNING("找到阀控板配置文件但缺少 'valve_control' 段，使用默认值");
                }
            } catch (const std::exception& e) {
                DEBUG_CORE_WARNING("无法加载阀控板配置文件: " << valve_config_file << "，使用主配置值: " << e.what());
            }
        }
        
        // 使用阀控板串口和波特率配置
        std::string valve_port = valve_config.getValveControlPort();
        int valve_baud = valve_config.getValveControlBaud();
        char valve_parity = valve_config.getValveControlParity();
        int valve_data_bits = valve_config.getValveControlDataBits();
        int valve_stop_bits = valve_config.getValveControlStopBits();
        
        // 优先使用配置的序列号查找设备
        std::string acquisition_card_port = smart_config_->getCurrentSerialPort();
        if (!valve_config.getValveControlSerialNumber().empty()) {
            // 配置了序列号，通过序列号查找设备（排除采集卡）
            std::vector<std::string> exclude_ports = {acquisition_card_port};
            std::string found_port = findDeviceBySerialNumber(valve_config.getValveControlSerialNumber(), exclude_ports);
            if (!found_port.empty()) {
                valve_port = found_port;
                DEBUG_CORE_LOG("通过配置的序列号找到阀控板设备: " << valve_config.getValveControlSerialNumber() << " -> " << valve_port);
            } else {
                DEBUG_CORE_WARNING("配置的序列号 " << valve_config.getValveControlSerialNumber() << " 未找到，使用配置的端口: " << valve_port);
            }
        }
        
        // 检查串口冲突：解析符号链接并检查是否与采集卡使用同一物理串口
        std::string valve_port_resolved = resolveSymbolicLink(valve_port);
        std::string acquisition_card_port_resolved = resolveSymbolicLink(acquisition_card_port);
        
        // 如果阀控板配置的是符号链接，优先使用符号链接解析后的实际设备
        // 这样可以确保使用正确的设备，而不是让智能检测随意选择
        if (valve_port != valve_port_resolved && !valve_port_resolved.empty()) {
            // 配置的是符号链接，检查解析后的设备是否与采集卡冲突
            if (valve_port_resolved == acquisition_card_port_resolved && !valve_port_resolved.empty()) {
                DEBUG_CORE_WARNING("端口冲突检测! 阀控板端口 " << valve_port 
                                  << " (" << valve_port_resolved << ") 与采集卡端口冲突 " 
                                  << acquisition_card_port << " (" << acquisition_card_port_resolved << ")");
                
                // 尝试从智能检测中选择其他可用设备
                if (smart_config_->isSmartDetectionEnabled()) {
                    auto detected_devices = smart_config_->getDetectedDevices();
                    std::string alternative_port = "";
                    int best_priority = -1;
                    
                    for (const auto& device : detected_devices) {
                        std::string device_resolved = resolveSymbolicLink(device.device_path);
                        // 跳过采集卡使用的串口
                        if (device_resolved == acquisition_card_port_resolved) {
                            continue;
                        }
                        
                        // 选择优先级最高的设备
                        if (device.priority > best_priority) {
                            best_priority = device.priority;
                            alternative_port = device.device_path;
                        }
                    }
                    
                    if (!alternative_port.empty()) {
                        valve_port = alternative_port;
                        DEBUG_CORE_LOG("自动选择阀控板备用端口: " << alternative_port);
                    } else {
                        DEBUG_CORE_ERROR("没有可用的阀控板备用端口，可能发生冲突！");
                    }
                } else {
                    DEBUG_CORE_ERROR("智能检测已禁用，无法自动选择阀控板备用端口！");
                }
            } else {
                // 没有冲突，使用符号链接解析后的实际设备
                valve_port = valve_port_resolved;
                DEBUG_CORE_LOG("使用解析后的符号链接作为阀控板端口: " << valve_port 
                              << " (来自 " << valve_config.getValveControlPort() << ")");
            }
        } else if (valve_port_resolved == acquisition_card_port_resolved && !valve_port_resolved.empty()) {
            // 配置的不是符号链接，但设备冲突
            DEBUG_CORE_WARNING("端口冲突检测! 阀控板端口 " << valve_port 
                              << " 与采集卡端口冲突 " << acquisition_card_port);
            
            // 尝试从智能检测中选择其他可用设备
            if (smart_config_->isSmartDetectionEnabled()) {
                auto detected_devices = smart_config_->getDetectedDevices();
                std::string alternative_port = "";
                int best_priority = -1;
                
                    for (const auto& device : detected_devices) {
                        std::string device_resolved = resolveSymbolicLink(device.device_path);
                        // 跳过采集卡使用的串口
                        if (device_resolved == acquisition_card_port_resolved) {
                            continue;
                        }
                    
                    // 选择优先级最高的设备
                    if (device.priority > best_priority) {
                        best_priority = device.priority;
                        alternative_port = device.device_path;
                    }
                }
                
                if (!alternative_port.empty()) {
                    valve_port = alternative_port;
                    DEBUG_CORE_LOG("自动选择阀控板备用端口: " << alternative_port);
                } else {
                    DEBUG_CORE_ERROR("没有可用的阀控板备用端口，可能发生冲突！");
                }
            } else {
                DEBUG_CORE_ERROR("智能检测已禁用，无法自动选择阀控板备用端口！");
            }
        }
        
        // 保存阀控板串口路径（用于热插拔检测）
        valve_control_port_ = valve_port;
        std::string valve_port_resolved_final = resolveSymbolicLink(valve_port);
        
        // 保存阀控板设备的 USB 序列号（用于重连识别）
        // 优先使用配置的序列号，如果没有配置则从设备读取
        if (!valve_config.getValveControlSerialNumber().empty()) {
            valve_control_serial_number_ = valve_config.getValveControlSerialNumber();
            DEBUG_CORE_LOG("阀控板设备序列号 (来自配置): " << valve_control_serial_number_);
        } else {
            valve_control_serial_number_ = getUSBSerialNumber(valve_port_resolved_final);
            if (!valve_control_serial_number_.empty()) {
                DEBUG_CORE_LOG("阀控板设备序列号 (来自设备): " << valve_control_serial_number_);
            }
        }
        
        // 将阀控板串口添加到排除列表，防止采集卡自动切换时占用
        smart_config_->addExcludedPort(valve_port);
        smart_config_->addExcludedPort(valve_port_resolved_final); // 同时排除解析后的路径
        
        // 获取阀控板从站ID列表（支持多从站）
        const auto& valve_slave_ids = valve_config.getValveControlSlaveIds();
        std::ostringstream slave_ids_str;
        for (size_t i = 0; i < valve_slave_ids.size(); ++i) {
            if (i > 0) slave_ids_str << ", ";
            slave_ids_str << valve_slave_ids[i];
        }
        
        DEBUG_CORE_LOG("正在初始化阀控板处理器，参数: 端口=" << valve_port 
                      << " (解析后: " << valve_port_resolved_final << ")"
                      << ", 波特率=" << valve_baud 
                      << ", 校验位=" << valve_parity
                      << ", 数据位=" << valve_data_bits
                      << ", 停止位=" << valve_stop_bits
                      << ", 从站ID=" << slave_ids_str.str());
        DEBUG_CORE_LOG("已将阀控板端口添加到排除列表以防止冲突");

        // 创建阀控板处理器，使用独立的串口和波特率，轮询间隔100ms
        valve_control_processor_ = std::make_unique<ValveControlProcessor>(
            valve_port, valve_baud, valve_parity, valve_data_bits, valve_stop_bits, valve_slave_ids, 100
        );

        if (!valve_control_processor_->initialize()) {
            DEBUG_CORE_ERROR("阀控板处理器初始化失败");
            return false;
        }

        DEBUG_CORE_LOG("阀控板处理器初始化成功");
        return true;
    } catch (const std::exception& e) {
        DEBUG_CORE_ERROR("阀控板处理器初始化失败: " << e.what());
        return false;
    }
}

bool ApplicationController::initializeJoystickManager() {
    try {
        joystick_manager_ = std::make_unique<JoystickManager>();

#ifdef ENABLE_ROS2
        if (!joystick_manager_->initialize(enable_ros2_, ros2_node_)) {
            DEBUG_CORE_LOG("摇杆管理器初始化失败");
            return false;
        }
        
        // 读取轴映射参数并设置
        if (enable_ros2_ && ros2_node_) {
            int linear_x = ros2_node_->declare_parameter("axis_mapping.linear_x", 1);
            int linear_y = ros2_node_->declare_parameter("axis_mapping.linear_y", -1);
            int linear_z = ros2_node_->declare_parameter("axis_mapping.linear_z", -1);
            int angular_x = ros2_node_->declare_parameter("axis_mapping.angular_x", -1);
            int angular_y = ros2_node_->declare_parameter("axis_mapping.angular_y", -1);
            int angular_z = ros2_node_->declare_parameter("axis_mapping.angular_z", 0);
            
            joystick_manager_->setAxisMapping(linear_x, linear_y, linear_z, angular_x, angular_y, angular_z);
            DEBUG_CORE_LOG("摇杆轴映射已从 ROS2 参数配置");
        }
#else
        if (!joystick_manager_->initialize(enable_ros2_, nullptr)) {
            DEBUG_CORE_LOG("摇杆管理器初始化失败");
            return false;
        }
#endif

        if (!joystick_manager_->start()) {
            DEBUG_CORE_LOG("无法启动摇杆管理器");
            return false;
        }

        DEBUG_CORE_LOG("摇杆管理器已初始化并成功启动");
        return true;
    } catch (const std::exception& e) {
        DEBUG_CORE_LOG("摇杆管理器初始化失败: " << e.what());
        return false;
    }
}

bool ApplicationController::initializeUSBSerialDetector() {
    try {
        usb_serial_detector_ = std::make_unique<USBSerialDetector>();
        
        // 设置设备状态变化回调
        usb_serial_detector_->setDeviceCallback([this](const std::string& device_path, bool connected) {
            // 解析设备路径（可能是符号链接）
            std::string device_path_resolved = resolveSymbolicLink(device_path);
            
            if (connected) {
                DEBUG_CORE_LOG("USB 串口设备已连接: " << device_path << " (解析后: " << device_path_resolved << ")");
                
                // 延迟处理，等待设备信息完全初始化
                std::thread([this, device_path, device_path_resolved]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    
                    // 获取配置中的序列号（优先使用配置的序列号）
                    const auto& config = smart_config_->getConfig();
                    std::string configured_main_serial = config.getSerialNumber();
                    std::string configured_valve_serial = config.getValveControlSerialNumber();
                    
                    // 获取新连接设备的 USB 序列号
                    std::string device_serial = getUSBSerialNumber(device_path_resolved);
                    
                    // 如果当前设备序列号匹配失败，扫描所有已连接设备查找匹配的序列号
                    bool acquisition_card_matched = false;
                    bool valve_control_matched = false;
                    
                    // 优先使用配置的序列号进行匹配（如果配置了）
                    // 检查是否是采集卡重新连接
                    if (modbus_client_) {
                        std::string expected_serial = !configured_main_serial.empty() ? configured_main_serial : acquisition_card_serial_number_;
                        if (!expected_serial.empty() && !device_serial.empty() && device_serial == expected_serial) {
                            acquisition_card_matched = true;
                            DEBUG_CORE_LOG("采集卡 USB 设备重新连接 (序列号: " << device_serial << "), 触发重连...");
                            // 更新序列号（如果从设备读取的序列号与配置不同，使用配置的序列号）
                            if (!configured_main_serial.empty()) {
                                acquisition_card_serial_number_ = configured_main_serial;
                            } else {
                                acquisition_card_serial_number_ = device_serial;
                            }
                            // 更新采集卡端口（因为路径可能已改变）
                            std::string current_port = smart_config_->getCurrentSerialPort();
                            if (device_path_resolved != resolveSymbolicLink(current_port)) {
                                DEBUG_CORE_LOG("更新采集卡端口: " << current_port << " -> " << device_path_resolved);
                                smart_config_->setSerialPort(device_path_resolved);
                                initializeModbus();
                            } else {
                    if (modbus_client_->isAutoReconnectEnabled()) {
                        modbus_client_->reconnect();
                    }
                }
                        }
                    }
                    
                    // 检查是否是阀控板设备重新连接（仅在采集卡未匹配时检查，避免冲突）
                    if (!acquisition_card_matched && valve_control_processor_) {
                        std::string expected_serial = !configured_valve_serial.empty() ? configured_valve_serial : valve_control_serial_number_;
                        if (!expected_serial.empty() && !device_serial.empty() && device_serial == expected_serial) {
                            valve_control_matched = true;
                            DEBUG_CORE_LOG("阀控板 USB 设备重新连接 (序列号: " << device_serial << "), 触发重连...");
                            // 更新序列号（如果从设备读取的序列号与配置不同，使用配置的序列号）
                            if (!configured_valve_serial.empty()) {
                                valve_control_serial_number_ = configured_valve_serial;
            } else {
                                valve_control_serial_number_ = device_serial;
                            }
                            // 更新阀控板端口（因为路径可能已改变）
                            if (device_path_resolved != resolveSymbolicLink(valve_control_port_)) {
                                DEBUG_CORE_LOG("更新阀控板端口: " << valve_control_port_ << " -> " << device_path_resolved);
                                valve_control_port_ = device_path_resolved;
                                // 更新阀控板处理器的端口（注意：不会触发采集卡重连，因为阀控板使用独立的 ModbusClient）
                                valve_control_processor_->updatePort(device_path_resolved);
                            } else {
                                // 端口未改变，只需重连
                                valve_control_processor_->reconnect();
                            }
                        }
                    }
                    
                    // 如果当前设备没有匹配，扫描所有已连接设备查找匹配的序列号
                    // 注意：只在当前连接的设备没有匹配到任何设备时才扫描，避免误触发已连接设备的重连
                    if (smart_config_->isSmartDetectionEnabled() && !acquisition_card_matched && !valve_control_matched) {
                        // 当前设备没有匹配到采集卡或阀控板，可能是新设备或需要重新匹配的设备
                        // 获取当前已连接的设备路径（用于排除，避免重复匹配已正常工作的设备）
                        std::string current_acquisition_port = smart_config_->getCurrentSerialPort();
                        std::string current_acquisition_port_resolved = resolveSymbolicLink(current_acquisition_port);
                        std::string current_valve_port_resolved = resolveSymbolicLink(valve_control_port_);
                        
                        auto detected_devices = smart_config_->getDetectedDevices();
                        for (const auto& device : detected_devices) {
                            std::string dev_path_resolved = resolveSymbolicLink(device.device_path);
                            
                            // 排除当前已连接的设备（如果设备已经连接且正常工作，不应该触发重连）
                            if (dev_path_resolved == current_acquisition_port_resolved || 
                                dev_path_resolved == current_valve_port_resolved ||
                                dev_path_resolved == device_path_resolved) {
                                continue; // 跳过已连接的设备
                            }
                            
                            std::string dev_serial = getUSBSerialNumber(dev_path_resolved);
                            
                            // 优先使用设备信息中的序列号
                            if (!device.serial_number.empty() && device.serial_number != "unknown") {
                                dev_serial = device.serial_number;
                            }
                            
                            // 检查采集卡（优先匹配）
                            if (!acquisition_card_matched && modbus_client_) {
                                std::string expected_serial = !configured_main_serial.empty() ? configured_main_serial : acquisition_card_serial_number_;
                                if (!expected_serial.empty() && !dev_serial.empty() && dev_serial == expected_serial) {
                                    acquisition_card_matched = true;
                                    DEBUG_CORE_LOG("采集卡 USB 设备在扫描中找到 (序列号: " << dev_serial << ", 路径: " << dev_path_resolved << "), 触发重连...");
                                    // 更新序列号（如果从设备读取的序列号与配置不同，使用配置的序列号）
                                    if (!configured_main_serial.empty()) {
                                        acquisition_card_serial_number_ = configured_main_serial;
                                    } else {
                                        acquisition_card_serial_number_ = dev_serial;
                                    }
                                    std::string current_port = smart_config_->getCurrentSerialPort();
                                    if (dev_path_resolved != resolveSymbolicLink(current_port)) {
                                        DEBUG_CORE_LOG("更新采集卡端口: " << current_port << " -> " << dev_path_resolved);
                                        smart_config_->setSerialPort(dev_path_resolved);
                                        initializeModbus();
                                    } else {
                                        if (modbus_client_->isAutoReconnectEnabled()) {
                                            modbus_client_->reconnect();
                                        }
                                    }
                                    continue; // 采集卡已匹配，跳过阀控板检查（一个设备不能同时是采集卡和阀控板）
                                }
                            }
                            
                            // 检查阀控板设备（仅在采集卡未匹配时检查，避免一个设备被同时匹配）
                            if (!acquisition_card_matched && !valve_control_matched && valve_control_processor_) {
                                std::string expected_serial = !configured_valve_serial.empty() ? configured_valve_serial : valve_control_serial_number_;
                                if (!expected_serial.empty() && !dev_serial.empty() && dev_serial == expected_serial) {
                                    valve_control_matched = true;
                                    DEBUG_CORE_LOG("阀控板 USB 设备在扫描中找到 (序列号: " << dev_serial << ", 路径: " << dev_path_resolved << "), 触发重连...");
                                    // 更新序列号（如果从设备读取的序列号与配置不同，使用配置的序列号）
                                    if (!configured_valve_serial.empty()) {
                                        valve_control_serial_number_ = configured_valve_serial;
                                    } else {
                                        valve_control_serial_number_ = dev_serial;
                                    }
                                    if (dev_path_resolved != resolveSymbolicLink(valve_control_port_)) {
                                        DEBUG_CORE_LOG("更新阀控板端口: " << valve_control_port_ << " -> " << dev_path_resolved);
                                        valve_control_port_ = dev_path_resolved;
                                        // 更新阀控板处理器的端口（注意：不会触发采集卡重连，因为阀控板使用独立的 ModbusClient）
                                        valve_control_processor_->updatePort(dev_path_resolved);
                                    } else {
                                        // 端口未改变，只需重连
                                        valve_control_processor_->reconnect();
                                    }
                                    continue; // 阀控板已匹配，跳过后续检查
                                }
                            }
                        }
                    }
                    
                    // 注意：所有重连都基于序列号匹配，不再使用路径匹配
                    // 这样可以避免设备热插拔时路径互换导致的错误匹配
                    if (!acquisition_card_matched && !valve_control_matched) {
                        // 如果序列号匹配都失败，说明当前设备不是采集卡或阀控板
                        // 不进行任何操作，等待正确的设备插入
                    }
                }).detach();
            } else {
                DEBUG_CORE_ERROR("USB 串口设备已断开: " << device_path << " (解析后: " << device_path_resolved << ")");
                
                // 优先使用序列号匹配（因为路径可能会变化）
                // 尝试获取断开设备的序列号
                std::string device_serial = getUSBSerialNumber(device_path_resolved);
                
                // 如果无法读取序列号，尝试路径匹配作为备选（但重连时仍然必须使用序列号）
                bool is_acquisition_card_by_path = false;
                bool is_valve_control_by_path = false;
                if (device_serial.empty()) {
                    // 无法读取序列号时，使用路径匹配作为辅助判断
                    std::string acquisition_card_port = smart_config_->getCurrentSerialPort();
                    std::string acquisition_card_port_resolved = resolveSymbolicLink(acquisition_card_port);
                    is_acquisition_card_by_path = (device_path == acquisition_card_port || device_path_resolved == acquisition_card_port_resolved);
                    
                    std::string valve_port_resolved = resolveSymbolicLink(valve_control_port_);
                    is_valve_control_by_path = (valve_control_processor_ && !valve_control_port_.empty() && 
                                                (device_path == valve_control_port_ || device_path_resolved == valve_port_resolved));
                }
                
                // 检查是否是采集卡断开（优先使用序列号，序列号不可用时使用路径）
                bool is_acquisition_card = false;
                if (!acquisition_card_serial_number_.empty() && !device_serial.empty()) {
                    // 使用序列号匹配
                    is_acquisition_card = (device_serial == acquisition_card_serial_number_);
                } else if (device_serial.empty() && is_acquisition_card_by_path) {
                    // 序列号不可用，使用路径匹配（但仅用于断开检测，重连时仍需要序列号）
                    is_acquisition_card = true;
                }
                
                if (modbus_client_ && is_acquisition_card) {
                    DEBUG_CORE_ERROR("采集卡断开连接" << 
                                    (!device_serial.empty() ? " (序列号: " + device_serial + ")" : " (路径匹配)") << 
                                    ", 等待 USB 重新连接...");
                    // 立即关闭Modbus连接
                    modbus_client_->close();
                    // 保留序列号，用于重连匹配
                }
                
                // 检查是否是阀控板设备断开（优先使用序列号，序列号不可用时使用路径）
                bool is_valve_control = false;
                if (!valve_control_serial_number_.empty() && !device_serial.empty()) {
                    // 使用序列号匹配
                    is_valve_control = (device_serial == valve_control_serial_number_);
                } else if (device_serial.empty() && is_valve_control_by_path) {
                    // 序列号不可用，使用路径匹配（但仅用于断开检测，重连时仍需要序列号）
                    is_valve_control = true;
                }
                
                if (valve_control_processor_ && is_valve_control) {
                    DEBUG_CORE_ERROR("阀控板设备断开连接" << 
                                    (!device_serial.empty() ? " (序列号: " + device_serial + ")" : " (路径匹配)") << 
                                    ", 等待 USB 重新连接...");
                    // 立即关闭阀控板连接
                    valve_control_processor_->close();
                    // 保留序列号，用于重连匹配
                }
            }
        });
        
        // 启动设备检测器
        if (!usb_serial_detector_->start(true)) {
            DEBUG_CORE_LOG("无法启动 USB 串口检测器");
            return false;
        }
        
        DEBUG_CORE_LOG("USB 串口检测器已初始化并成功启动");
        return true;
    } catch (const std::exception& e) {
        DEBUG_CORE_LOG("USB 串口检测器初始化失败: " << e.what());
        return false;
    }
}

void ApplicationController::registerSignalHandlers() {
    std::signal(SIGINT, onSignal);   // Ctrl+C
    std::signal(SIGTERM, onSignal);  // 终止信号
}

void ApplicationController::onSignal(int sig) {
    (void)sig;
    g_run.store(false, std::memory_order_relaxed);
}

void ApplicationController::runModbusMode() {
    DEBUG_CORE_LOG("正在以 Modbus + USB 摇杆模式运行");
    
    const auto period = std::chrono::duration<double>(1.0 / poll_hz_);

    while (g_run.load(std::memory_order_relaxed)) {
        auto t0 = std::chrono::steady_clock::now();

        // 处理采集卡数据
        if (!acquisition_card_processor_->processData()) {
            // 处理失败，睡到下个周期继续尝试
            auto dt = std::chrono::steady_clock::now() - t0;
            if (dt < period) std::this_thread::sleep_for(period - dt);
            continue;
        }

#ifdef ENABLE_ROS2
        // 在Modbus模式下也处理ROS2消息
        if (ros2_initialized_ && ros2_node_) {
            rclcpp::spin_some(ros2_node_);
        }
#endif

        // 控制循环频率：睡到下一个周期
        auto dt = std::chrono::steady_clock::now() - t0;
        if (dt < period) std::this_thread::sleep_for(period - dt);
    }
}

void ApplicationController::runUSBOnlyMode() {
    DEBUG_CORE_LOG("正在以仅 USB 摇杆模式运行");
    
    while (g_run.load(std::memory_order_relaxed)) {
#ifdef ENABLE_ROS2
        if (ros2_initialized_ && ros2_node_) {
            // ROS2 spin once，处理消息发布
            rclcpp::spin_some(ros2_node_);
        }
#endif

        // 只运行手柄检测器，每JOYSTICK_SCAN_INTERVAL_MS检查一次状态
        std::this_thread::sleep_for(std::chrono::milliseconds(JOYSTICK_SCAN_INTERVAL_MS));

        // 定期显示连接状态
        static int counter = 0;
        if (++counter >= STATUS_DISPLAY_INTERVAL_COUNT) { // 每STATUS_DISPLAY_TIME_MS显示一次状态
            counter = 0;
            int device_count = joystick_manager_->getConnectedDeviceCount();
            DEBUG_JOYSTICK_LOG("USB 摇杆模式: " << device_count << " 个设备已连接");
        }
    }
}

void ApplicationController::runDynamicMode() {
    if (enable_modbus_) {
        DEBUG_CORE_LOG("正在以热插拔单模式检测运行 (Modbus 或 USB，不同时)");
    } else {
        DEBUG_CORE_LOG("正在以仅 USB 摇杆模式运行 (Modbus 已禁用)");
    }
    
    const auto period = std::chrono::duration<double>(1.0 / poll_hz_);
    bool last_modbus_state = modbus_connected_;
    int last_usb_count = 0;
    std::string current_mode = "standby";
    
    while (g_run.load(std::memory_order_relaxed)) {
        auto t0 = std::chrono::steady_clock::now();
        
        // 检查Modbus连接状态变化（仅在启用modbus时）
        bool current_modbus_state = false;
        if (enable_modbus_ && modbus_client_) {
            current_modbus_state = modbus_client_->ok();
        }
        
        // 立即检测并报告Modbus连接状态变化
        if (enable_modbus_ && current_modbus_state != last_modbus_state) {
            if (current_modbus_state) {
                DEBUG_MODBUS_COMM_LOG("🔗 ========== 采集卡已连接 ========== 🔗");
            } else {
                DEBUG_MODBUS_ERROR("❌ ========== 采集卡已断开 ========== ❌");
            }
            last_modbus_state = current_modbus_state;
        }
        
        int current_usb_count = joystick_manager_ ? joystick_manager_->getConnectedDeviceCount() : 0;
        
        // 模式切换逻辑：如果modbus启用，优先使用modbus模式（即使暂时断开也保持modbus模式等待重连）
        std::string new_mode = "standby";
        if (enable_modbus_) {
            new_mode = "modbus";  // 启用modbus时始终使用modbus模式，支持自动重连
        } else if (current_usb_count > 0) {
            new_mode = "usb";
        }
        
        // 检查是否需要切换模式
        if (new_mode != current_mode) {
            DEBUG_CORE_LOG("模式切换: " << current_mode << " -> " << new_mode);
            
            // 清理当前模式资源
            cleanupCurrentMode(current_mode);
            
            // 初始化新模式资源
            if (!initializeMode(new_mode)) {
                DEBUG_CORE_ERROR("无法初始化模式: " << new_mode);
                new_mode = "standby";
            }
            
            current_mode = new_mode;
            modbus_connected_ = (current_mode == "modbus");
        }
        
        if (current_usb_count != last_usb_count) {
            DEBUG_JOYSTICK_LOG("USB 摇杆数量已更改: " << last_usb_count << " -> " << current_usb_count);
            last_usb_count = current_usb_count;
        }
        
        // 根据当前模式处理数据
        bool processing_ok = true;
        if (current_mode == "modbus" && enable_modbus_ && acquisition_card_processor_) {
            processing_ok = acquisition_card_processor_->processData();
            if (!processing_ok) {
                // 只在第一次处理失败时打印日志，避免重复刷屏
                static bool last_processing_failed = false;
                if (!last_processing_failed) {
                    DEBUG_MODBUS_ERROR("Modbus 处理失败，正在检查连接");
                }
                last_processing_failed = true;
            } else {
                // 重置状态，允许下次打印
                static bool last_processing_failed = false;
                last_processing_failed = false;
            }

            // 轮询读取阀控板状态（使用功能码03读取保持寄存器）
            // 注意：只有在 enable_valve_control=true 时，valve_control_processor_ 才会被创建
            if (valve_control_processor_) {
                valve_control_processor_->pollValveStatus();
                
#ifdef ENABLE_ROS2
                // 设置ROS2节点（用于订阅摇杆话题）
                if (ros2_node_) {
                    static bool ros2_node_set = false;
                    if (!ros2_node_set) {
                        valve_control_processor_->setROS2Node(ros2_node_);
                        ros2_node_set = true;
                    }
                }
#endif
            }
        }
        // USB模式的数据处理由JoystickManager自动处理
        
        // 处理ROS2消息
#ifdef ENABLE_ROS2
        if (ros2_initialized_ && ros2_node_) {
            // 多次调用spin_some确保消息被发送（ROS2的publish是异步的）
            for (int i = 0; i < 3; ++i) {
                rclcpp::spin_some(ros2_node_);
            }
            
            // 定期发布心跳和设备状态
            static auto last_heartbeat_publish = std::chrono::steady_clock::now();
            auto now = std::chrono::steady_clock::now();
            auto time_since_last = std::chrono::duration_cast<std::chrono::seconds>(now - last_heartbeat_publish);
            if (time_since_last >= std::chrono::seconds(HEARTBEAT_INTERVAL_SECONDS)) {
                // 发布系统心跳状态
                publishSystemHeartbeat();
                
                // 发布所有已连接的设备状态
                if (joystick_manager_) {
                    auto devices = joystick_manager_->getConnectedDevices();
                    for (const auto& device : devices) {
                        joystick_manager_->publishDeviceStatus("CONNECTED", device.device_path);
                    }
                }
                
                // 发布后立即spin，确保消息被发送
                for (int i = 0; i < 3; ++i) {
                    rclcpp::spin_some(ros2_node_);
                }
                
                last_heartbeat_publish = now;
            }
        }
#endif
        
        // 控制循环频率
        auto dt = std::chrono::steady_clock::now() - t0;
        if (dt < period) {
            std::this_thread::sleep_for(period - dt);
        }
        
        // 状态信息只在模式切换时打印，不进行定期打印
    }
}

void ApplicationController::cleanupCurrentMode(const std::string& mode) {
    DEBUG_CORE_LOG("正在清理模式: " << mode);
    
    if (mode == "modbus") {
        // 清理Modbus模式资源
        if (uinput_created_ && uinput_device_) {
            const auto& config = smart_config_->getConfig();
            // 安全清零所有输出
            for (auto& a : config.getAxes()) {
                uinput_device_->sendAbs(a.code, 0);
            }
            for (auto& d : config.getDiscreteAxes()) {
                uinput_device_->sendAbs(d.code, 0);
            }
            for (auto& b : config.getButtons()) {
                uinput_device_->sendKey(b.code, 0);
            }
            uinput_device_->sync();
        }
        
        // 清理采集卡处理器
        acquisition_card_processor_.reset();
        
        // 清理uinput设备
        uinput_device_.reset();
        uinput_created_ = false;
        
    } else if (mode == "usb") {
        // USB模式不需要特殊清理，JoystickManager会自动处理
        DEBUG_CORE_LOG("USB 模式清理完成");
        
    } else if (mode == "standby") {
        // 待机模式清理所有资源
        DEBUG_CORE_LOG("待机模式清理完成");
    }
}

bool ApplicationController::initializeMode(const std::string& mode) {
    DEBUG_CORE_LOG("正在初始化模式: " << mode);
    
    if (mode == "modbus") {
        // 暂停USB手柄数据处理
        if (joystick_manager_) {
            joystick_manager_->pause();
        }
        
        // 初始化Modbus模式
        if (!uinput_created_) {
            if (!initializeUInput()) {
                DEBUG_UINPUT_ERROR("无法为 Modbus 模式初始化 uinput");
#ifdef ENABLE_ROS2
                // 如果启用了ROS2，即使uinput失败也可以继续运行（仅ROS2输出）
                if (enable_ros2_) {
                    DEBUG_CORE_LOG("uinput不可用，将仅使用ROS2输出模式");
                } else {
                    DEBUG_CORE_ERROR("uinput不可用且未启用ROS2，无法继续运行");
                    return false;
                }
#else
                // 未启用ROS2且uinput失败，无法继续
                DEBUG_CORE_ERROR("uinput不可用且未启用ROS2，无法继续运行");
                return false;
#endif
            }
        }
        
        if (!acquisition_card_processor_) {
            if (!initializeAcquisitionCardProcessor()) {
                DEBUG_CORE_ERROR("采集卡处理器初始化失败");
                return false;
            }
        }
        
        DEBUG_CORE_LOG("Modbus 模式初始化成功");
        return true;
        
    } else if (mode == "usb") {
        // 恢复USB手柄数据处理
        if (joystick_manager_) {
            joystick_manager_->resume();
        }
        
        DEBUG_CORE_LOG("USB 模式初始化成功");
        return true;
        
    } else if (mode == "standby") {
        // 待机模式暂停USB手柄数据处理
        if (joystick_manager_) {
            joystick_manager_->pause();
        }
        
        DEBUG_CORE_LOG("待机模式初始化成功");
        return true;
    }
    
    DEBUG_CORE_ERROR("未知模式: " << mode);
    return false;
}

void ApplicationController::cleanup() {
    DEBUG_CORE_LOG("正在清理资源...");

    // 停止手柄管理器
    if (joystick_manager_) {
        joystick_manager_->stop();
    }

    // 停止USB转串口设备检测器
    if (usb_serial_detector_) {
        usb_serial_detector_->stop();
    }

    // 退出前安全清零（仅在Modbus模式下需要）
    if (uinput_created_ && uinput_device_) {
        const auto& config = smart_config_->getConfig();
        for (auto& a : config.getAxes()) {
            uinput_device_->sendAbs(a.code, 0);
        }
        for (auto& d : config.getDiscreteAxes()) {
            uinput_device_->sendAbs(d.code, 0);
        }
        for (auto& b : config.getButtons()) {
            uinput_device_->sendKey(b.code, 0);
        }
        uinput_device_->sync();

        // 显式释放底层资源，避免依赖析构时机
        uinput_device_->destroy();        // 销毁虚拟设备（热插拔友好）
    }

    // 关闭Modbus连接
    if (modbus_connected_ && modbus_client_) {
        modbus_client_->close();
    }

#ifdef ENABLE_ROS2
    // 清理ROS2
    if (ros2_initialized_) {
        rclcpp::shutdown();
        DEBUG_CORE_LOG("ROS2 已关闭");
    }
#endif

    DEBUG_CORE_LOG("清理完成");
}

std::string ApplicationController::resolveSymbolicLink(const std::string& path) const {
    if (path.empty()) {
        return path;
    }
    
    // 使用 realpath 解析符号链接
    char resolved_path[PATH_MAX];
    if (realpath(path.c_str(), resolved_path) != nullptr) {
        return std::string(resolved_path);
    }
    
    // 如果 realpath 失败，尝试使用 readlink
    char link_target[PATH_MAX];
    ssize_t len = readlink(path.c_str(), link_target, sizeof(link_target) - 1);
    if (len > 0) {
        link_target[len] = '\0';
        // 如果是相对路径，需要转换为绝对路径
        if (link_target[0] != '/') {
            // 获取符号链接所在目录
            size_t last_slash = path.find_last_of('/');
            if (last_slash != std::string::npos) {
                std::string dir = path.substr(0, last_slash + 1);
                return dir + std::string(link_target);
            }
        }
        return std::string(link_target);
    }
    
    // 如果都失败，返回原路径
    return path;
}

std::string ApplicationController::getUSBSerialNumber(const std::string& device_path) const {
    // 从设备路径提取设备名称
    std::string device_name = device_path.substr(device_path.find_last_of('/') + 1);
    
    // 尝试从/sys/class/tty读取USB序列号
    // USB串口设备的序列号路径：/sys/class/tty/{device}/device/../../serial
    // 或者：/sys/class/tty/{device}/device/../serial
    std::vector<std::string> possible_paths = {
        "/sys/class/tty/" + device_name + "/device/../../serial",
        "/sys/class/tty/" + device_name + "/device/../serial",
        "/sys/class/tty/" + device_name + "/device/serial"
    };
    
    for (const auto& sys_path : possible_paths) {
        std::ifstream serial_file(sys_path);
        if (serial_file.is_open()) {
            std::string serial_number;
            std::getline(serial_file, serial_number);
            serial_file.close();
            // 去除可能的空白字符
            if (!serial_number.empty()) {
                // 去除首尾空白
                serial_number.erase(0, serial_number.find_first_not_of(" \t\n\r"));
                serial_number.erase(serial_number.find_last_not_of(" \t\n\r") + 1);
                if (!serial_number.empty() && serial_number != "unknown") {
                    return serial_number;
                }
            }
        }
    }
    
    // 如果无法读取，返回空字符串
    return "";
}

std::string ApplicationController::findDeviceBySerialNumber(const std::string& serial_number, const std::vector<std::string>& exclude_ports) const {
    if (serial_number.empty()) {
        return "";
    }
    
    // 如果智能检测未启用，无法查找
    if (!smart_config_->isSmartDetectionEnabled()) {
        return "";
    }
    
    // 获取所有检测到的设备
    auto detected_devices = smart_config_->getDetectedDevices();
    
    // 构建排除路径集合（包括解析后的路径）
    std::set<std::string> exclude_set;
    for (const auto& port : exclude_ports) {
        exclude_set.insert(port);
        std::string resolved = resolveSymbolicLink(port);
        if (!resolved.empty() && resolved != port) {
            exclude_set.insert(resolved);
        }
    }
    
    // 遍历所有设备，查找匹配的序列号
    for (const auto& device : detected_devices) {
        // 检查是否在排除列表中
        std::string device_resolved = resolveSymbolicLink(device.device_path);
        if (exclude_set.find(device.device_path) != exclude_set.end() ||
            exclude_set.find(device_resolved) != exclude_set.end()) {
            continue;
        }
        
        // 获取设备的序列号
        std::string dev_serial = getUSBSerialNumber(device_resolved);
        
        // 如果设备信息中已有序列号，优先使用
        if (!device.serial_number.empty() && device.serial_number != "unknown") {
            dev_serial = device.serial_number;
        }
        
        // 匹配序列号
        if (!dev_serial.empty() && dev_serial == serial_number) {
            DEBUG_CORE_LOG("通过序列号找到设备: " << serial_number << " -> " << device.device_path);
            return device.device_path;
        }
    }
    
    // 未找到匹配的设备
    DEBUG_CORE_WARNING("未找到序列号为 " << serial_number << " 的设备");
    return "";
}

std::string ApplicationController::getRunMode() const {
    if (enable_modbus_ && modbus_connected_) {
        return "Hot-swap Single Mode (Modbus + ROS2)";
    } else {
        int usb_count = joystick_manager_ ? joystick_manager_->getConnectedDeviceCount() : 0;
        if (usb_count > 0) {
            if (enable_modbus_) {
                return "Hot-swap Single Mode (USB + ROS2, Modbus disabled)";
            } else {
                return "USB Joystick Only Mode (ROS2)";
            }
        } else {
            if (enable_modbus_) {
                return "Hot-swap Single Mode (Standby + ROS2, Modbus disabled)";
            } else {
                return "USB Joystick Only Mode (Standby + ROS2)";
            }
        }
    }
}

void ApplicationController::publishSystemHeartbeat() {
#ifdef ENABLE_ROS2
    if (!ros2_initialized_ || !ros2_node_) {
        return;
    }

    // 创建心跳状态发布者（如果还没有创建）
    static rclcpp::Publisher<std_msgs::msg::String>::SharedPtr heartbeat_pub = nullptr;
    if (!heartbeat_pub) {
        // 使用默认QoS（Reliable），与其他发布者保持一致，确保ros2 topic echo可以接收
        heartbeat_pub = ros2_node_->create_publisher<std_msgs::msg::String>(HEARTBEAT_TOPIC_NAME, 10);
        
        // 等待一段时间让ROS2发现机制完成
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // 强制宣布发布者（帮助ROS2发现机制）
        if (heartbeat_pub) {
            // 发布一个空消息来触发发现
            auto dummy_msg = std_msgs::msg::String();
            dummy_msg.data = "";
            heartbeat_pub->publish(dummy_msg);
            // 立即spin确保消息被处理
            for (int i = 0; i < 5; ++i) {
                rclcpp::spin_some(ros2_node_);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    // 构建心跳数据
    auto heartbeat_msg = std_msgs::msg::String();
    
    // 获取当前时间戳
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    // 构建JSON格式的心跳数据
    std::ostringstream heartbeat_data;
    heartbeat_data << "{"
                   << "\"" << HEARTBEAT_FIELD_TIMESTAMP << "\":" << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() << ","
                   << "\"" << HEARTBEAT_FIELD_SYSTEM_STATUS << "\":\"" << HEARTBEAT_STATUS_RUNNING << "\","
                   << "\"" << HEARTBEAT_FIELD_RUN_MODE << "\":\"" << getRunMode() << "\","
                   << "\"" << HEARTBEAT_FIELD_MODBUS_ENABLED << "\":" << (enable_modbus_ ? JSON_TRUE : JSON_FALSE) << ","
                   << "\"" << HEARTBEAT_FIELD_MODBUS_CONNECTED << "\":" << (modbus_connected_ ? JSON_TRUE : JSON_FALSE) << ","
                   << "\"" << HEARTBEAT_FIELD_ROS2_ENABLED << "\":" << (enable_ros2_ ? JSON_TRUE : JSON_FALSE) << ","
                   << "\"" << HEARTBEAT_FIELD_ROS2_INITIALIZED << "\":" << (ros2_initialized_ ? JSON_TRUE : JSON_FALSE) << ","
                   << "\"" << HEARTBEAT_FIELD_USB_DEVICES_COUNT << "\":" << (joystick_manager_ ? joystick_manager_->getConnectedDeviceCount() : 0) << ","
                   << "\"" << HEARTBEAT_FIELD_POLLING_RATE << "\":" << poll_hz_ << ","
                   << "\"" << HEARTBEAT_FIELD_UPTIME << "\":" << std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() - std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count()
                   << "}";
    
    heartbeat_msg.data = heartbeat_data.str();
    heartbeat_pub->publish(heartbeat_msg);
    
    // 发布后立即spin，确保消息被发送（ROS2发布是异步的）
    // 多次spin确保消息被处理
    for (int i = 0; i < 3; ++i) {
        rclcpp::spin_some(ros2_node_);
    }
    
    // 显示订阅者数量变化（用于监控ROS2连接状态）
    size_t subscription_count = heartbeat_pub->get_subscription_count();
    static size_t last_subscription_count = 0;
    if (subscription_count != last_subscription_count) {
        DEBUG_CORE_LOG("心跳订阅数量已更改: " << last_subscription_count << " -> " << subscription_count);
        last_subscription_count = subscription_count;
    }
#endif
}
