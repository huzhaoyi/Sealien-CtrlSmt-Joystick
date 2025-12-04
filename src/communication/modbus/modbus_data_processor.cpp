/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-09-08 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-09-08 11:29:37
 * @FilePath: src/core/modbus_data_processor.cpp
 * @Description: Modbus数据处理器实现文件
 * 负责处理Modbus数据的读取、解析和转换
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "modbus_data_processor.h"
#include "constants.h"
#include "utils.h"
#include "filters.h"
#include "mapping.h"
#include "byte_order.h"
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>

using json = nlohmann::json;

ModbusDataProcessor::ModbusDataProcessor(Config& config, ModbusClient& modbus_client, UInputDevice* uinput_device, 
                                         rclcpp::Node::SharedPtr ros2_node)
    : config_(config), modbus_client_(modbus_client), uinput_device_(uinput_device), ros2_node_(ros2_node) {
#ifdef ENABLE_ROS2
    modbus_device_path_ = "modbus_joystick";
#endif
}

bool ModbusDataProcessor::initialize() {
    try {
        // 设置Modbus寄存器最大值（用于电压转换）
        modbus_client_.setModbusMaxValue(config_.getModbusMaxValue());
        
        // 收集地址集合
        collectAddressSets();
        
        // 计算地址范围
        computeRanges();
        
#ifdef ENABLE_ROS2
        // 初始化ROS2发布者（如果提供了ROS2节点）
        if (ros2_node_) {
            joy_pub_ = ros2_node_->create_publisher<sensor_msgs::msg::Joy>(ROS2_TOPIC_JOY, ROS2_TOPIC_QUEUE_SIZE);
            twist_pub_ = ros2_node_->create_publisher<geometry_msgs::msg::Twist>(ROS2_TOPIC_TWIST, ROS2_TOPIC_QUEUE_SIZE);
            status_pub_ = ros2_node_->create_publisher<std_msgs::msg::String>(ROS2_TOPIC_STATUS, ROS2_TOPIC_QUEUE_SIZE);
            modbus_raw_pub_ = ros2_node_->create_publisher<std_msgs::msg::String>(ROS2_TOPIC_MODBUS_RAW, ROS2_TOPIC_QUEUE_SIZE);
            config_pub_ = ros2_node_->create_publisher<std_msgs::msg::String>(ROS2_TOPIC_CONFIG, ROS2_TOPIC_QUEUE_SIZE);
            control_sub_ = ros2_node_->create_subscription<std_msgs::msg::String>(
                ROS2_TOPIC_MODBUS_CONTROL, ROS2_TOPIC_QUEUE_SIZE,
                std::bind(&ModbusDataProcessor::modbusControlCallback, this, std::placeholders::_1));
            control_response_pub_ = ros2_node_->create_publisher<std_msgs::msg::String>(ROS2_TOPIC_MODBUS_CONTROL, ROS2_TOPIC_QUEUE_SIZE);
        } else {
            DEBUG_CORE_ERROR("ModbusDataProcessor: ros2_node_ is null, cannot create ROS2 publishers!");
        }
#endif
        
        initialized_ = true;
        return true;
    } catch (const std::exception& e) {
        DEBUG_CORE_LOG("ModbusDataProcessor initialization failed: " << e.what());
        return false;
    }
}

void ModbusDataProcessor::setAxisMapping(int linear_x_axis, int linear_y_axis, int linear_z_axis,
                                        int angular_x_axis, int angular_y_axis, int angular_z_axis) {
    axis_mapping_.linear_x = linear_x_axis;
    axis_mapping_.linear_y = linear_y_axis;
    axis_mapping_.linear_z = linear_z_axis;
    axis_mapping_.angular_x = angular_x_axis;
    axis_mapping_.angular_y = angular_y_axis;
    axis_mapping_.angular_z = angular_z_axis;
}

bool ModbusDataProcessor::processData() {
    if (!initialized_) {
        DEBUG_CORE_LOG("ModbusDataProcessor not initialized");
        return false;
    }

    // 检查USB设备是否存在，如果不存在则停止所有Modbus操作
    if (!modbus_client_.checkDeviceExists()) {
        // 只在第一次检测到设备不存在时打印日志，避免重复刷屏
        static bool last_device_unavailable = false;
        if (!last_device_unavailable) {
            DEBUG_MODBUS_COMM_LOG("USB device not available, stopping Modbus operations");
        }
        last_device_unavailable = true;
        
        // 清零所有输出并发布ROS2消息
        clearAllOutputs();
#ifdef ENABLE_ROS2
        if (ros2_node_) {
            publishModbusROS2Messages();
            publishRawModbusData();
        }
#endif
        return false;
    }
    
    // 重置状态，允许下次打印
    static bool last_device_unavailable = false;
    last_device_unavailable = false;

    // 初始化数据缓存映射（合并所有从站的数据）
    std::map<int, uint16_t> in_map, ho_map;    // 输入寄存器和保持寄存器
    std::map<int, uint8_t> co_map;             // 线圈
    std::map<int, uint8_t> di_map;             // 离散量输入

    // 获取所有从站ID列表
    const auto& slave_ids = config_.getSlaveIds();
    bool read_ok = false;
    
    // 存储每个从站的数据（使用从站ID作为键）
    std::map<int, std::map<int, uint16_t> > slave_in_maps, slave_ho_maps;
    std::map<int, std::map<int, uint8_t> > slave_co_maps, slave_di_maps;
    
    // 轮询所有从站
    // 在多从站模式下，需要统计成功和失败的从站数量
    int success_count = 0;
    int fail_count = 0;
    std::vector<int> failed_slave_ids;  // 记录失败的从站ID
    
    // 在多从站模式下，临时禁用自动更新总线错误状态，由外层统一处理
    bool original_auto_update = modbus_client_.isAutoUpdateBusError();
    if (slave_ids.size() > 1) {
        modbus_client_.setAutoUpdateBusError(false);
    }
    
    for (int slave_id : slave_ids) {
        // 切换从站ID
        if (!modbus_client_.setSlaveId(slave_id)) {
            // 在多从站模式下，只记录失败的从站ID，稍后统一打印
            if (slave_ids.size() > 1) {
                failed_slave_ids.push_back(slave_id);
            } else {
                DEBUG_MODBUS_ERROR("Failed to set slave ID to " << slave_id);
            }
            fail_count++;
            continue;
        }
        
        // 读取当前从站的数据
        std::map<int, uint16_t> slave_in_map, slave_ho_map;
        std::map<int, uint8_t> slave_co_map, slave_di_map;
        
        bool slave_read_ok = readAllData(slave_in_map, slave_ho_map, slave_co_map, slave_di_map);
        
        if (slave_read_ok) {
            success_count++;
            read_ok = true;  // 至少有一个从站读取成功
            
            // 保存当前从站的数据（使用从站ID作为键）
            slave_in_maps[slave_id] = slave_in_map;
            slave_ho_maps[slave_id] = slave_ho_map;
            slave_co_maps[slave_id] = slave_co_map;
            slave_di_maps[slave_id] = slave_di_map;
            
            // 同时合并到总映射中（用于向后兼容，如果只有一个从站）
            for (const auto& [addr, value] : slave_in_map) {
                in_map[addr] = value;
            }
            for (const auto& [addr, value] : slave_ho_map) {
                ho_map[addr] = value;
            }
            for (const auto& [addr, value] : slave_co_map) {
                co_map[addr] = value;
            }
            for (const auto& [addr, value] : slave_di_map) {
                di_map[addr] = value;
            }
        } else {
            fail_count++;
            // 在多从站模式下，记录失败的从站ID
            if (slave_ids.size() > 1) {
                failed_slave_ids.push_back(slave_id);
            }
        }
    }
    
    // 在多从站模式下，汇总打印失败信息
    if (slave_ids.size() > 1 && !failed_slave_ids.empty()) {
        for (int failed_id : failed_slave_ids) {
            DEBUG_MODBUS_ERROR("Slave[" << failed_id << "] read failed");
        }
    }
    
    // 恢复自动更新总线错误状态的设置
    if (slave_ids.size() > 1) {
        modbus_client_.setAutoUpdateBusError(original_auto_update);
        
        // 在多从站模式下，只有在所有从站都读取失败时才更新总线错误状态
        // 只有在所有从站都读取成功时才清除总线错误状态
        // 这样可以避免因为部分从站失败而频繁输出"Bus error cleared"消息
        if (fail_count == static_cast<int>(slave_ids.size())) {
            // 所有从站都失败，更新总线错误状态
            modbus_client_.updateBusErrorStatus(false);
        } else if (success_count == static_cast<int>(slave_ids.size())) {
            // 所有从站都成功，清除总线错误状态
            modbus_client_.updateBusErrorStatus(true);
        }
        // 部分成功部分失败的情况，不更新总线错误状态，避免频繁输出消息
    }
    // 单从站模式下，readAllData 内部已经更新了总线错误状态，这里不需要额外处理
    
    // 如果配置了多个从站，需要根据配置中的从站ID和 ros2_joy_index 来区分数据
    // 将每个从站的数据传递给处理函数，让它们根据 ros2_joy_index 正确映射

    // 更新原始Modbus数据（无论读取成功与否）
#ifdef ENABLE_ROS2
    if (ros2_node_) {
        updateRawModbusData(in_map, ho_map, co_map, di_map, read_ok);
    }
#endif

    // 断线/读失败安全策略：清零所有输出
    if (!read_ok) {
        clearAllOutputs();
        
        // 检查总线错误状态
        if (modbus_client_.isBusError()) {
            // 只在第一次检测到总线错误时尝试重连，避免重复尝试
            static bool last_bus_error_state = false;
            if (!last_bus_error_state) {
                DEBUG_MODBUS_ERROR(modbus_client_.getBusErrorInfo());
                
                // 如果启用了自动重连，尝试重连（总线错误重连）
                if (modbus_client_.isAutoReconnectEnabled()) {
                    DEBUG_MODBUS_ERROR("Attempting automatic reconnection...");
                    if (modbus_client_.reconnect(true)) {  // 传递true表示这是总线错误重连
                        DEBUG_MODBUS_COMM_LOG("Automatic reconnection successful");
                    } else {
                        DEBUG_MODBUS_ERROR("Automatic reconnection failed");
                    }
                }
            }
            last_bus_error_state = true;
        } else {
            static bool last_bus_error_state = false;
            static auto last_clear_report_time = std::chrono::steady_clock::now();
            
            if (last_bus_error_state) {
                // 第一次清除时立即报告（使用信息级别，因为这是好消息）
                DEBUG_MODBUS_COMM_LOG("Bus error cleared, communication restored");
                last_clear_report_time = std::chrono::steady_clock::now();
            } else {
                // 定期报告通信正常状态（每1秒报告一次）
                auto now = std::chrono::steady_clock::now();
                auto time_since_last_report = std::chrono::duration_cast<std::chrono::seconds>(now - last_clear_report_time);
                if (time_since_last_report.count() >= 1) {
                    DEBUG_MODBUS_COMM_LOG("Modbus communication normal");
                    last_clear_report_time = now;
                }
            }
            last_bus_error_state = false;
        }
        
        // 发布清零的ROS2消息
#ifdef ENABLE_ROS2
        if (ros2_node_) {
            publishModbusROS2Messages();
            publishRawModbusData();
        } else {
            DEBUG_CORE_ERROR("ros2_node_ is null, cannot publish cleared ROS2 messages");
        }
#endif
        return false;
    }

    // 处理连续轴（如果配置了多个从站，使用分从站的数据）
    if (slave_ids.size() > 1 && !slave_in_maps.empty()) {
        // 多从站模式：为每个从站分别处理数据
        processAxesMultiSlave(slave_in_maps, slave_ho_maps);
    } else {
        // 单从站模式：使用合并后的数据（向后兼容）
        processAxes(in_map, ho_map);
    }

    // 处理离散轴
    processDiscreteAxes(in_map, ho_map, co_map, di_map);

    // 处理按钮
    processButtons(in_map, ho_map, co_map, di_map);

    // 同步所有输入事件
    if (uinput_device_) {
        uinput_device_->sync();
    }

#ifdef ENABLE_ROS2
    // 发布Modbus设备ROS2消息
    if (ros2_node_) {
        publishModbusROS2Messages();
        publishRawModbusData();
        publishConfigData();
    } else {
        DEBUG_CORE_ERROR("ros2_node_ is null, cannot publish ROS2 messages");
    }
#endif

    return true;
}

void ModbusDataProcessor::clearAllOutputs() {
    if (!uinput_device_) return;
    
    for (auto& a : config_.getAxes()) {
        uinput_device_->sendAbs(a.code, 0);
    }
    for (auto& d : config_.getDiscreteAxes()) {
        uinput_device_->sendAbs(d.code, 0);
    }
    for (auto& b : config_.getButtons()) {
        uinput_device_->sendKey(b.code, 0);
    }
    uinput_device_->sync();
    
#ifdef ENABLE_ROS2
    // 清零ROS2数据
    {
        std::lock_guard<std::mutex> lock(modbus_data_mutex_);
        modbus_device_data_.axes.resize(MAX_JOYSTICK_AXES, 0.0f);
        modbus_device_data_.buttons.resize(MAX_JOYSTICK_BUTTONS, 0);
    }
#endif
}

void ModbusDataProcessor::collectAddressSets() {
    // 清空现有集合
    need_in_regs_.clear();
    need_hold_regs_.clear();
    need_coils_.clear();
    need_discrete_inputs_.clear();

    // 收集轴地址
    for (auto& a : config_.getAxes()) {
        if (a.src.kind == AxisSourceKind::InputRegister) {
            need_in_regs_.insert(a.src.addr);
            // 如果是32位浮点数（engineeringValue，地址范围800-814，偶数地址），也需要读取下一个地址
            // 注意：地址800实际存储低字，地址801实际存储高字（与手册标记相反）
            bool is_float32 = (a.src.addr >= 800 && a.src.addr <= 814 && (a.src.addr % 2 == 0));
            if (is_float32) {
                need_in_regs_.insert(a.src.addr + 1);  // 添加高字地址（实际存储位置）
            }
        } else {
            need_hold_regs_.insert(a.src.addr);
        }
    }

    // 收集按钮地址
    for (auto& b : config_.getButtons()) {
        if (b.src.kind == ButtonSourceKind::Coil) {
            need_coils_.insert(b.src.addr);
        } else if (b.src.kind == ButtonSourceKind::DiscreteInput) {
            need_discrete_inputs_.insert(b.src.addr);
        } else if (b.src.kind == ButtonSourceKind::InputRegisterBit) {
            need_in_regs_.insert(b.src.addr);
        } else {
            need_hold_regs_.insert(b.src.addr);
        }
    }

    // 收集离散轴地址
    for (auto& d : config_.getDiscreteAxes()) {
        auto add_src = [&](const RegBitSource& s) {
            if (s.kind == ButtonSourceKind::Coil) {
                need_coils_.insert(s.addr);
            } else if (s.kind == ButtonSourceKind::DiscreteInput) {
                need_discrete_inputs_.insert(s.addr);
            } else if (s.kind == ButtonSourceKind::InputRegisterBit) {
                need_in_regs_.insert(s.addr);
            } else {
                need_hold_regs_.insert(s.addr);
            }
        };
        add_src(d.up);
        add_src(d.down);
    }
}

void ModbusDataProcessor::computeRanges() {
    ranges_.in_min = ranges_.in_max = -1;
    ranges_.ho_min = ranges_.ho_max = -1;
    ranges_.co_min = ranges_.co_max = -1;

    if (!need_in_regs_.empty()) {
        auto [min_val, max_val] = minmaxAddr<uint16_t>(need_in_regs_);
        ranges_.in_min = min_val;
        ranges_.in_max = max_val;
    }

    if (!need_hold_regs_.empty()) {
        auto [min_val, max_val] = minmaxAddr<uint16_t>(need_hold_regs_);
        ranges_.ho_min = min_val;
        ranges_.ho_max = max_val;
    }

    if (!need_coils_.empty()) {
        auto [min_val, max_val] = minmaxAddr<uint8_t>(need_coils_);
        ranges_.co_min = min_val;
        ranges_.co_max = max_val;
    }

    // 离散量输入使用与线圈相同的地址范围
    if (!need_discrete_inputs_.empty()) {
        auto [di_min, di_max] = minmaxAddr<uint8_t>(need_discrete_inputs_);
        if (ranges_.co_min == -1) {
            ranges_.co_min = di_min;
            ranges_.co_max = di_max;
        } else {
            ranges_.co_min = std::min(ranges_.co_min, di_min);
            ranges_.co_max = std::max(ranges_.co_max, di_max);
        }
    }
}

bool ModbusDataProcessor::readAllData(std::map<int, uint16_t>& in_map,
                                      std::map<int, uint16_t>& ho_map,
                                      std::map<int, uint8_t>& co_map,
                                      std::map<int, uint8_t>& di_map,
                                      bool /* update_bus_error */) {
    bool read_ok = true;
    bool has_required_data = false;  // 标记是否有必需的数据被成功读取

    // 读取输入寄存器（对于Moxa R1240，这是必需的）
    if (ranges_.in_max >= ranges_.in_min) {
        std::vector<uint16_t> v;
        int count = ranges_.in_max - ranges_.in_min + 1;
        if (modbus_client_.readInputRange(ranges_.in_min, count, v)) {
            for (int i = 0; i < count; ++i) {
                in_map[ranges_.in_min + i] = v[i];
            }
            has_required_data = true;  // 输入寄存器读取成功，标记为有必需数据
        } else {
            read_ok = false;
            // 获取当前从站ID用于错误打印
            int current_slave_id = modbus_client_.getSlaveId();
            DEBUG_MODBUS_ERROR("Slave[" << current_slave_id << "] failed to read input registers: addr=" << ranges_.in_min << ", count=" << count);
        }
    } else {
        DEBUG_CORE_ERROR("Invalid input register range: in_min=" << ranges_.in_min << ", in_max=" << ranges_.in_max);
    }

    // 读取保持寄存器（可选，失败不影响整体）
    if (ranges_.ho_max >= ranges_.ho_min) {
        std::vector<uint16_t> v;
        int count = ranges_.ho_max - ranges_.ho_min + 1;
        if (modbus_client_.readHoldingRange(ranges_.ho_min, count, v)) {
            for (int i = 0; i < count; ++i) {
                ho_map[ranges_.ho_min + i] = v[i];
            }
        }
        // 保持寄存器读取失败不影响整体状态（不设置read_ok=false）
    }

    // 读取线圈（可选，失败不影响整体）
    if (ranges_.co_max >= ranges_.co_min) {
        std::vector<uint8_t> v;
        int count = ranges_.co_max - ranges_.co_min + 1;
        if (modbus_client_.readCoilRange(ranges_.co_min, count, v)) {
            for (int i = 0; i < count; ++i) {
                co_map[ranges_.co_min + i] = v[i] ? 1 : 0;
            }
        }
        // 线圈读取失败不影响整体状态

        // 同时读取离散量输入（使用相同的地址范围，可选）
        if (modbus_client_.readDiscreteInputRange(ranges_.co_min, count, v)) {
            for (int i = 0; i < count; ++i) {
                di_map[ranges_.co_min + i] = v[i] ? 1 : 0;
            }
        }
        // 离散量输入读取失败不影响整体状态
    }

    // 只要有必需的数据（输入寄存器）被成功读取，就返回true
    // 线圈和离散量输入读取失败不影响整体状态
    // 注意：具体的错误信息已经在readInputRange中打印，这里不再重复打印
    return has_required_data;
}

void ModbusDataProcessor::processAxes(const std::map<int, uint16_t>& in_map,
                                      const std::map<int, uint16_t>& ho_map) {
    // 计算动态Modbus值范围
    auto [modbus_min, modbus_max] = calculateModbusRange();

#ifdef ENABLE_ROS2
    // 更新ROS2轴数据缓存
    {
        std::lock_guard<std::mutex> lock(modbus_data_mutex_);
        modbus_device_data_.axes.resize(MAX_JOYSTICK_AXES, 0.0f);
    }
#endif

    size_t axis_index = 0;
    for (auto& a : config_.getAxes()) {
        double x = 0.0;  // 默认值
        bool is_abnormal = false;
        
        // 检查是否是32位浮点数（engineeringValue，地址范围800-814，偶数地址）
        // 注意：虽然手册中地址800标记为"high word"，但实际存储格式是：地址800=低字，地址801=高字
        // engineeringValue地址：AI-00=800(low实际),801(high实际), AI-01=802(low实际),803(high实际), ...
        bool is_float32 = (a.src.addr >= 800 && a.src.addr <= 814 && (a.src.addr % 2 == 0));
        
        if (is_float32 && a.src.kind == AxisSourceKind::InputRegister) {
            // 读取32位浮点数（两个word）
            // Moxa R1240的engineeringValue格式：地址800存储低字，地址801存储高字（与手册标记相反）
            uint16_t low_word = GetRegValue(in_map, a.src.addr, 0);      // 地址800：实际是低字
            uint16_t high_word = GetRegValue(in_map, a.src.addr + 1, 0); // 地址801：实际是高字
            
            // 组合成32位整数：高字在前，低字在后（标准IEEE 754格式）
            uint32_t uint32_value = ((uint32_t)high_word << 16) | low_word;
            
            // 应用字节序转换（如果需要）
            if (a.swap_endian) {
                uint32_value = ByteOrder::swapBytes32(uint32_value);
            }
            
            // 转换为浮点数（IEEE 754 Big-Endian格式）
            float float_value;
            std::memcpy(&float_value, &uint32_value, sizeof(float));
            double engineering_value = static_cast<double>(float_value);
            
            // 检查是否为有效浮点数
            if (!std::isfinite(engineering_value)) {
                // 无效浮点数，设置为中心位置
                x = 0.0;
                is_abnormal = true;
            } else {
                // 使用配置文件中的电压映射参数进行归一化
                double min_v = config_.getMinVoltage();
                double mid_v = config_.getMidVoltage();
                double max_v = config_.getMaxVoltage();
                
                // 检查engineeringValue是否在有效范围内（0-10V）
                if (engineering_value < 0.0 || engineering_value > 10.0) {
                    // 超出范围，设置为中心位置
                    x = 0.0;
                    is_abnormal = true;
                } else {
                    // 使用分段线性映射：min_voltage -> -1, mid_voltage -> 0, max_voltage -> 1
                    // 添加容差：当值在mid_voltage附近（±0.01V）时，直接映射到0
                    const double tolerance = 0.01;  // 容差：0.01V
                    if (std::abs(engineering_value - mid_v) <= tolerance) {
                        // 在mid_voltage附近，直接映射到0
                        x = 0.0;
                    } else if (engineering_value < mid_v) {
                        // 在min_voltage到mid_voltage之间：映射到[-1, 0]
                        if (mid_v > min_v) {
                            double ratio = (engineering_value - min_v) / (mid_v - min_v);
                            x = ratio * 1.0 - 1.0;  // -1.0 到 0.0
                        } else {
                            x = 0.0;
                        }
                    } else {
                        // 在mid_voltage到max_voltage之间：映射到[0, 1]
                        if (max_v > mid_v) {
                            double ratio = (engineering_value - mid_v) / (max_v - mid_v);
                            x = ratio * 1.0;  // 0.0 到 1.0
                        } else {
                            x = 0.0;
                        }
                    }
                }
            }
        } else {
            // 16位整数处理（原有逻辑）
            uint16_t raw_uint = (a.src.kind == AxisSourceKind::InputRegister)
                                ? GetRegValue(in_map, a.src.addr, 0)
                                : GetRegValue(ho_map, a.src.addr, 0);

            // 应用字节序转换（输入寄存器需要字节序转换）
            uint16_t fixed_raw = a.swap_endian ? ByteOrder::fixModbusValue(raw_uint) : raw_uint;

            // 转换为有符号整数
            int16_t raw_signed = ByteOrder::toSigned16(fixed_raw);
            int raw = static_cast<int>(raw_signed);

            // 检测读取失败的情况
            if (raw_uint == 0) {
                x = 0.0;  // 读取失败时设置为中心位置
                is_abnormal = true;
            }
            // 检测异常值（值过小，可能是设备错误）
            else if (raw_uint < 100) {  // 正常模拟量值应该大于100
                x = 0.0;  // 异常值时设置为中心位置
                is_abnormal = true;
            }
            // 正常值处理
            else {
                // 使用动态计算的Modbus值范围进行归一化
                double norm = (std::clamp(raw, static_cast<int>(modbus_min), static_cast<int>(modbus_max)) - modbus_min)
                              / std::max(1.0, (double)(modbus_max - modbus_min));
                x = norm * 2.0 - 1.0;
            }
        }
        if (a.invert) x = -x;
        x = apply_deadzone(x, a.deadzone);
        double y = a.lpf.process(x);
        int js = (int)std::lround(std::clamp(y, -1.0, 1.0) * (double)AXIS_MAX_VALUE);
        if (uinput_device_) {
            uinput_device_->sendAbs(a.code, js);
        }

#ifdef ENABLE_ROS2
        // 更新ROS2轴数据（根据 ros2_joy_index 映射到正确的位置）
        if (a.ros2_joy_index >= 0 && a.ros2_joy_index < MAX_JOYSTICK_AXES) {
            std::lock_guard<std::mutex> lock(modbus_data_mutex_);
            modbus_device_data_.axes[a.ros2_joy_index] = static_cast<float>(y);
        }
#endif
        axis_index++;
    }
}

void ModbusDataProcessor::processAxesMultiSlave(const std::map<int, std::map<int, uint16_t>>& slave_in_maps,
                                                const std::map<int, std::map<int, uint16_t>>& slave_ho_maps) {
    // 计算动态Modbus值范围
    auto [modbus_min, modbus_max] = calculateModbusRange();

#ifdef ENABLE_ROS2
    // 更新ROS2轴数据缓存
    {
        std::lock_guard<std::mutex> lock(modbus_data_mutex_);
        modbus_device_data_.axes.resize(MAX_JOYSTICK_AXES, 0.0f);
    }
#endif

    // 获取所有从站ID列表（按顺序）
    const auto& slave_ids = config_.getSlaveIds();
    
    // 计算每个从站的轴数量（假设所有从站的轴数量相同）
    auto& axes = config_.getAxes();  // 需要非const引用，因为lpf.process()不是const方法
    if (axes.empty() || slave_ids.empty()) {
        return;
    }
    
    int axes_per_slave = axes.size() / slave_ids.size();
    if (axes_per_slave == 0) {
        axes_per_slave = axes.size();  // 如果只有一个从站，使用全部轴
    }
    
    size_t axis_index = 0;
    
    // 遍历所有轴配置
    for (size_t i = 0; i < axes.size(); ++i) {
        auto& a = axes[i];  // 需要非const引用，因为lpf.process()不是const方法
        
        // 确定当前轴属于哪个从站（根据轴索引和每个从站的轴数量）
        int slave_index = (axes.size() > slave_ids.size()) ? (i / axes_per_slave) : 0;
        if (slave_index >= static_cast<int>(slave_ids.size())) {
            slave_index = slave_ids.size() - 1;
        }
        
        int slave_id = slave_ids[slave_index];
        
        // 获取对应从站的数据映射
        const std::map<int, uint16_t>* in_map = nullptr;
        const std::map<int, uint16_t>* ho_map = nullptr;
        
        auto it_in = slave_in_maps.find(slave_id);
        if (it_in != slave_in_maps.end()) {
            in_map = &it_in->second;
        }
        
        auto it_ho = slave_ho_maps.find(slave_id);
        if (it_ho != slave_ho_maps.end()) {
            ho_map = &it_ho->second;
        }
        
        if (!in_map && !ho_map) {
            // 该从站没有数据，跳过
            axis_index++;
            continue;
        }
        
        double x = 0.0;  // 默认值
        bool is_abnormal = false;
        
        // 检查是否是32位浮点数（engineeringValue，地址范围800-814，偶数地址）
        bool is_float32 = (a.src.addr >= 800 && a.src.addr <= 814 && (a.src.addr % 2 == 0));
        
        if (is_float32 && a.src.kind == AxisSourceKind::InputRegister && in_map) {
            // 读取32位浮点数（两个word）
            uint16_t low_word = GetRegValue(*in_map, a.src.addr, 0);
            uint16_t high_word = GetRegValue(*in_map, a.src.addr + 1, 0);
            
            uint32_t uint32_value = ((uint32_t)high_word << 16) | low_word;
            
            if (a.swap_endian) {
                uint32_value = ByteOrder::swapBytes32(uint32_value);
            }
            
            float float_value;
            std::memcpy(&float_value, &uint32_value, sizeof(float));
            double engineering_value = static_cast<double>(float_value);
            
            if (!std::isfinite(engineering_value)) {
                x = 0.0;
                is_abnormal = true;
            } else {
                double min_v = config_.getMinVoltage();
                double mid_v = config_.getMidVoltage();
                double max_v = config_.getMaxVoltage();
                
                if (engineering_value < 0.0 || engineering_value > 10.0) {
                    x = 0.0;
                    is_abnormal = true;
                } else {
                    const double tolerance = 0.01;
                    if (std::abs(engineering_value - mid_v) <= tolerance) {
                        x = 0.0;
                    } else if (engineering_value < mid_v) {
                        if (mid_v > min_v) {
                            double ratio = (engineering_value - min_v) / (mid_v - min_v);
                            x = ratio * 1.0 - 1.0;
                        } else {
                            x = 0.0;
                        }
                    } else {
                        if (max_v > mid_v) {
                            double ratio = (engineering_value - mid_v) / (max_v - mid_v);
                            x = ratio * 1.0;
                        } else {
                            x = 0.0;
                        }
                    }
                }
            }
        } else {
            // 16位整数处理
            uint16_t raw_uint = 0;
            if (a.src.kind == AxisSourceKind::InputRegister && in_map) {
                raw_uint = GetRegValue(*in_map, a.src.addr, 0);
            } else if (a.src.kind == AxisSourceKind::HoldingRegister && ho_map) {
                raw_uint = GetRegValue(*ho_map, a.src.addr, 0);
            }
            
            uint16_t fixed_raw = a.swap_endian ? ByteOrder::fixModbusValue(raw_uint) : raw_uint;
            int16_t raw_signed = ByteOrder::toSigned16(fixed_raw);
            int raw = static_cast<int>(raw_signed);
            
            if (raw_uint == 0) {
                x = 0.0;
                is_abnormal = true;
            } else if (raw_uint < 100) {
                x = 0.0;
                is_abnormal = true;
            } else {
                double norm = (std::clamp(raw, static_cast<int>(modbus_min), static_cast<int>(modbus_max)) - modbus_min)
                              / std::max(1.0, (double)(modbus_max - modbus_min));
                x = norm * 2.0 - 1.0;
            }
        }
        
        if (a.invert) x = -x;
        x = apply_deadzone(x, a.deadzone);
        double y = a.lpf.process(x);
        int js = (int)std::lround(std::clamp(y, -1.0, 1.0) * (double)AXIS_MAX_VALUE);
        if (uinput_device_) {
            uinput_device_->sendAbs(a.code, js);
        }

#ifdef ENABLE_ROS2
        // 更新ROS2轴数据（根据 ros2_joy_index 映射到正确的位置）
        if (a.ros2_joy_index >= 0 && a.ros2_joy_index < MAX_JOYSTICK_AXES) {
            std::lock_guard<std::mutex> lock(modbus_data_mutex_);
            modbus_device_data_.axes[a.ros2_joy_index] = static_cast<float>(y);
        }
#endif
        axis_index++;
    }
}

void ModbusDataProcessor::processDiscreteAxes(const std::map<int, uint16_t>& in_map,
                                             const std::map<int, uint16_t>& ho_map,
                                             const std::map<int, uint8_t>& co_map,
                                             const std::map<int, uint8_t>& di_map) {
    auto get_bit = [&](const RegBitSource& s) -> int {
        if (s.kind == ButtonSourceKind::Coil) {
            return GetCoilValue(co_map, s.addr, 0);
        } else if (s.kind == ButtonSourceKind::DiscreteInput) {
            return GetCoilValue(di_map, s.addr, 0);
        } else if (s.kind == ButtonSourceKind::InputRegisterBit) {
            return (GetRegValue(in_map, s.addr, 0) >> s.bit) & 1;
        } else {
            return (GetRegValue(ho_map, s.addr, 0) >> s.bit) & 1;
        }
    };

    size_t discrete_axis_index = MAX_JOYSTICK_STICKS; // 从第5个轴开始（索引4）
    for (auto& d : config_.getDiscreteAxes()) {
        int up = get_bit(d.up);
        int dn = get_bit(d.down);
        double x = (up ? 1.0 : 0.0) - (dn ? 1.0 : 0.0);
        double y = d.lpf.process(x);
        int js = (int)std::lround(std::clamp(y, -1.0, 1.0) * (double)AXIS_MAX_VALUE);
        if (uinput_device_) {
            uinput_device_->sendAbs(d.code, js);
        }

#ifdef ENABLE_ROS2
        // 更新ROS2旋钮数据（后8个轴为旋钮）
        if (discrete_axis_index < MAX_JOYSTICK_AXES) {
            std::lock_guard<std::mutex> lock(modbus_data_mutex_);
            modbus_device_data_.axes[discrete_axis_index] = static_cast<float>(y);
        }
#endif
        discrete_axis_index++;
    }
}

void ModbusDataProcessor::processButtons(const std::map<int, uint16_t>& in_map,
                                        const std::map<int, uint16_t>& ho_map,
                                        const std::map<int, uint8_t>& co_map,
                                        const std::map<int, uint8_t>& di_map) {
    auto get_bit = [&](const RegBitSource& s) -> int {
        if (s.kind == ButtonSourceKind::Coil) {
            return GetCoilValue(co_map, s.addr, 0);
        } else if (s.kind == ButtonSourceKind::DiscreteInput) {
            return GetCoilValue(di_map, s.addr, 0);
        } else if (s.kind == ButtonSourceKind::InputRegisterBit) {
            return (GetRegValue(in_map, s.addr, 0) >> s.bit) & 1;
        } else {
            return (GetRegValue(ho_map, s.addr, 0) >> s.bit) & 1;
        }
    };

    auto now = std::chrono::steady_clock::now();
    
#ifdef ENABLE_ROS2
    // 更新ROS2按钮数据缓存
    {
        std::lock_guard<std::mutex> lock(modbus_data_mutex_);
        modbus_device_data_.buttons.resize(MAX_JOYSTICK_BUTTONS, 0);
    }
#endif

    for (auto& b : config_.getButtons()) {
        int bit = get_bit(b.src);
        int level = b.active_high ? bit : !bit;
        if (b.debouncer.update(level, now)) {
            if (uinput_device_) {
                uinput_device_->sendKey(b.code, b.debouncer.stable());
            }
            
#ifdef ENABLE_ROS2
            // 更新ROS2按钮数据（根据 ros2_joy_index 映射到正确的位置）
            if (b.ros2_joy_index >= 0 && b.ros2_joy_index < MAX_JOYSTICK_BUTTONS) {
                std::lock_guard<std::mutex> lock(modbus_data_mutex_);
                modbus_device_data_.buttons[b.ros2_joy_index] = b.debouncer.stable();
            }
#endif
        }
    }
}

template<typename T>
std::pair<int, int> ModbusDataProcessor::minmaxAddr(const std::set<int>& s) {
    if (s.empty()) return {0, -1};
    return {*s.begin(), *s.rbegin()};
}

uint16_t ModbusDataProcessor::voltageToModbusValue(double voltage) {
    // 使用配置的Modbus最大值范围进行转换
    uint16_t modbus_max = config_.getModbusMaxValue();
    double normalized = std::clamp(voltage / 10.0, 0.0, 1.0);
    return static_cast<uint16_t>(normalized * modbus_max);
}

std::pair<uint16_t, uint16_t> ModbusDataProcessor::calculateModbusRange() {
    // 使用配置的Modbus最大值范围进行转换
    uint16_t modbus_max = config_.getModbusMaxValue();
    uint16_t min_modbus = static_cast<uint16_t>((config_.getMinVoltage() / 10.0) * modbus_max);
    uint16_t max_modbus = static_cast<uint16_t>((config_.getMaxVoltage() / 10.0) * modbus_max);
    return {min_modbus, max_modbus};
}

#ifdef ENABLE_ROS2
void ModbusDataProcessor::publishModbusROS2Messages() {
    if (!ros2_node_) {
        DEBUG_CORE_ERROR("publishModbusROS2Messages: ros2_node_ is null");
        return;
    }
    
    // 检查发布者是否已初始化
    if (!joy_pub_) {
        DEBUG_CORE_ERROR("publishModbusROS2Messages: joy_pub_ is null");
    }
    if (!twist_pub_) {
        DEBUG_CORE_ERROR("publishModbusROS2Messages: twist_pub_ is null");
    }
    
    // 更新Modbus设备数据
    {
        std::lock_guard<std::mutex> lock(modbus_data_mutex_);
        auto now = ros2_node_->now();
        modbus_device_data_.last_update = now;
        
        // 确保轴和按钮数组大小统一：12个轴，32个按钮
        // 统一格式：前4个轴为摇杆，后8个轴为旋钮，32个按钮/开关
        modbus_device_data_.axes.resize(MAX_JOYSTICK_AXES, 0.0f);
        modbus_device_data_.buttons.resize(MAX_JOYSTICK_BUTTONS, 0);
        
        // 轴数据已经在processAxes和processDiscreteAxes中更新，这里不需要重新设置
        // 确保轴数组大小正确
        modbus_device_data_.axes.resize(MAX_JOYSTICK_AXES, 0.0f);
        
        // 按钮数据已经在processButtons中更新，这里不需要重新设置
        // 确保按钮数组大小正确
        modbus_device_data_.buttons.resize(MAX_JOYSTICK_BUTTONS, 0);
    }
    
    // 发布Joy消息
    if (joy_pub_) {
        auto joy_msg = sensor_msgs::msg::Joy();
        joy_msg.header.stamp = modbus_device_data_.last_update;
        joy_msg.header.frame_id = modbus_device_path_;
        
        // 初始化Joy消息数组
        joy_msg.axes.resize(MAX_JOYSTICK_AXES, 0.0f);
        joy_msg.buttons.resize(MAX_JOYSTICK_BUTTONS, 0);
        
        // 根据配置的轴索引映射设置轴数据
        // 注意：modbus_device_data_.axes 中的数据已经根据 ros2_joy_index 存储
        // 所以这里直接使用 modbus_device_data_.axes 中的数据即可
        for (int i = 0; i < MAX_JOYSTICK_AXES && i < static_cast<int>(modbus_device_data_.axes.size()); ++i) {
            joy_msg.axes[i] = modbus_device_data_.axes[i];
        }
        
        // 根据配置的按钮索引映射设置按钮数据
        // 注意：modbus_device_data_.buttons 中的数据已经根据 ros2_joy_index 存储
        // 所以这里直接使用 modbus_device_data_.buttons 中的数据即可
        for (int i = 0; i < MAX_JOYSTICK_BUTTONS && i < static_cast<int>(modbus_device_data_.buttons.size()); ++i) {
            joy_msg.buttons[i] = modbus_device_data_.buttons[i];
        }
        
        joy_pub_->publish(joy_msg);
    }
    
    // 发布Twist消息
    if (twist_pub_) {
        auto twist_msg = geometry_msgs::msg::Twist();
        
        // 使用配置的轴映射设置Twist消息
        if (axis_mapping_.linear_x >= 0 && axis_mapping_.linear_x < static_cast<int>(modbus_device_data_.axes.size())) {
            twist_msg.linear.x = modbus_device_data_.axes[axis_mapping_.linear_x];
        }
        if (axis_mapping_.linear_y >= 0 && axis_mapping_.linear_y < static_cast<int>(modbus_device_data_.axes.size())) {
            twist_msg.linear.y = modbus_device_data_.axes[axis_mapping_.linear_y];
        }
        if (axis_mapping_.linear_z >= 0 && axis_mapping_.linear_z < static_cast<int>(modbus_device_data_.axes.size())) {
            twist_msg.linear.z = modbus_device_data_.axes[axis_mapping_.linear_z];
        }
        if (axis_mapping_.angular_x >= 0 && axis_mapping_.angular_x < static_cast<int>(modbus_device_data_.axes.size())) {
            twist_msg.angular.x = modbus_device_data_.axes[axis_mapping_.angular_x];
        }
        if (axis_mapping_.angular_y >= 0 && axis_mapping_.angular_y < static_cast<int>(modbus_device_data_.axes.size())) {
            twist_msg.angular.y = modbus_device_data_.axes[axis_mapping_.angular_y];
        }
        if (axis_mapping_.angular_z >= 0 && axis_mapping_.angular_z < static_cast<int>(modbus_device_data_.axes.size())) {
            twist_msg.angular.z = modbus_device_data_.axes[axis_mapping_.angular_z];
        }
        
        twist_pub_->publish(twist_msg);
    }
}

void ModbusDataProcessor::updateRawModbusData(const std::map<int, uint16_t>& in_map,
                                            const std::map<int, uint16_t>& ho_map,
                                            const std::map<int, uint8_t>& co_map,
                                            const std::map<int, uint8_t>& di_map,
                                            bool connection_status) {
    std::lock_guard<std::mutex> lock(raw_modbus_data_mutex_);
    
    auto now = ros2_node_->now();
    raw_modbus_data_.last_update = now;
    raw_modbus_data_.connection_status = connection_status;
    
    // 更新输入寄存器数据
    raw_modbus_data_.input_registers = in_map;
    
    // 更新保持寄存器数据
    raw_modbus_data_.holding_registers = ho_map;
    
    // 更新线圈数据
    raw_modbus_data_.coils = co_map;
    
    // 更新离散量输入数据
    raw_modbus_data_.discrete_inputs = di_map;
    
    // 计算电压值（从输入寄存器转换）
    raw_modbus_data_.voltages.clear();
    uint16_t modbus_max = modbus_client_.getModbusMaxValue();
    for (const auto& [addr, value] : in_map) {
        // 将Modbus值转换为电压值（使用配置的Modbus最大值范围）
        double voltage = (static_cast<double>(value) / static_cast<double>(modbus_max)) * 10.0;
        raw_modbus_data_.voltages[addr] = voltage;
    }
}

void ModbusDataProcessor::publishRawModbusData() {
    if (!ros2_node_) {
        DEBUG_CORE_ERROR("publishRawModbusData: ros2_node_ is null");
        return;
    }
    if (!modbus_raw_pub_) {
        DEBUG_CORE_ERROR("publishRawModbusData: modbus_raw_pub_ is null");
        return;
    }
    
    std::lock_guard<std::mutex> lock(raw_modbus_data_mutex_);
    
    // 创建JSON格式的原始Modbus数据
    json raw_data;
    raw_data["timestamp"] = raw_modbus_data_.last_update.seconds();
    raw_data["connection_status"] = raw_modbus_data_.connection_status;
    
    // 输入寄存器数据
    json input_regs;
    for (const auto& [addr, value] : raw_modbus_data_.input_registers) {
        input_regs[std::to_string(addr)] = value;
    }
    raw_data["input_registers"] = input_regs;
    
    // 保持寄存器数据
    json holding_regs;
    for (const auto& [addr, value] : raw_modbus_data_.holding_registers) {
        holding_regs[std::to_string(addr)] = value;
    }
    raw_data["holding_registers"] = holding_regs;
    
    // 线圈数据
    json coils_data;
    for (const auto& [addr, value] : raw_modbus_data_.coils) {
        coils_data[std::to_string(addr)] = static_cast<bool>(value);
    }
    raw_data["coils"] = coils_data;
    
    // 离散量输入数据
    json discrete_inputs;
    for (const auto& [addr, value] : raw_modbus_data_.discrete_inputs) {
        discrete_inputs[std::to_string(addr)] = static_cast<bool>(value);
    }
    raw_data["discrete_inputs"] = discrete_inputs;
    
    // 电压数据
    json voltages;
    for (const auto& [addr, voltage] : raw_modbus_data_.voltages) {
        voltages[std::to_string(addr)] = voltage;
    }
    raw_data["voltages"] = voltages;
    
    // 发布JSON字符串
    auto msg = std_msgs::msg::String();
    msg.data = raw_data.dump();
    modbus_raw_pub_->publish(msg);
}

void ModbusDataProcessor::publishConfigData() {
    if (!config_pub_) {
        DEBUG_CORE_ERROR("publishConfigData: config_pub_ is null");
        return;
    }
    
    // 构建配置信息JSON
    json config_data;
    
    // 串口配置
    config_data["serial_config"] = {
        {"port", config_.getSerialPort()},
        {"baudrate", config_.getBaud()},
        {"data_bits", config_.getDataBits()},
        {"stop_bits", config_.getStopBits()},
        {"parity", std::string(1, config_.getParity())},
        {"slave_id", config_.getSlaveId()}
    };
    
    // 电压映射配置
    config_data["voltage_mapping"] = {
        {"min_voltage", config_.getMinVoltage()},
        {"mid_voltage", config_.getMidVoltage()},
        {"max_voltage", config_.getMaxVoltage()}
    };
    
    // 轴配置（从第一个轴获取默认值）
    if (!config_.getAxes().empty()) {
        const auto& first_axis = config_.getAxes()[0];
        config_data["axis_config"] = {
            {"deadzone", first_axis.deadzone},
            {"lpf_alpha", first_axis.lpf_alpha},
            {"polling_rate", config_.getPollHz()}
        };
    } else {
        config_data["axis_config"] = {
            {"deadzone", 0.12},
            {"lpf_alpha", 1.0},
            {"polling_rate", config_.getPollHz()}
        };
    }
    
    // 系统配置
    config_data["system_config"] = {
        {"bus_error_threshold", config_.getBusErrorThreshold()},
        {"total_axes", static_cast<int>(config_.getAxes().size())},
        {"total_buttons", static_cast<int>(config_.getButtons().size())},
        {"total_discrete_axes", static_cast<int>(config_.getDiscreteAxes().size())}
    };
    
    // 设备映射配置
    json axes_mapping = json::array();
    for (const auto& axis : config_.getAxes()) {
        json axis_info = {
            {"code", axis.code},
            {"source_type", axis.src.kind == AxisSourceKind::InputRegister ? "input_register" : "holding_register"},
            {"source_addr", axis.src.addr},
            {"raw_min", axis.raw_min},
            {"raw_max", axis.raw_max},
            {"deadzone", axis.deadzone},
            {"lpf_alpha", axis.lpf_alpha},
            {"invert", axis.invert},
            {"swap_endian", axis.swap_endian},
            {"ros2_joy_index", axis.ros2_joy_index}
        };
        axes_mapping.push_back(axis_info);
    }
    config_data["axes_mapping"] = axes_mapping;
    
    json buttons_mapping = json::array();
    for (const auto& button : config_.getButtons()) {
        json button_info = {
            {"code", button.code},
            {"source_type", [&]() {
                switch (button.src.kind) {
                    case ButtonSourceKind::Coil: return "coil";
                    case ButtonSourceKind::DiscreteInput: return "discrete_input";
                    case ButtonSourceKind::InputRegisterBit: return "input_register_bit";
                    case ButtonSourceKind::HoldingRegisterBit: return "holding_register_bit";
                    default: return "unknown";
                }
            }()},
            {"source_addr", button.src.addr},
            {"source_bit", button.src.bit},
            {"active_high", button.active_high},
            {"debounce_ms", 10}, // 默认去抖时间
            {"ros2_joy_index", button.ros2_joy_index}
        };
        buttons_mapping.push_back(button_info);
    }
    config_data["buttons_mapping"] = buttons_mapping;
    
    // ROS2话题配置
    config_data["ros2_config"] = {
        {"joy_topic", ROS2_TOPIC_JOY},
        {"twist_topic", ROS2_TOPIC_TWIST},
        {"status_topic", ROS2_TOPIC_STATUS},
        {"modbus_raw_topic", ROS2_TOPIC_MODBUS_RAW},
        {"config_topic", ROS2_TOPIC_CONFIG},
        {"modbus_control_topic", ROS2_TOPIC_MODBUS_CONTROL}
    };
    
    // 添加时间戳
    config_data["timestamp"] = std::time(nullptr);
    
    // 发布JSON字符串
    auto msg = std_msgs::msg::String();
    msg.data = config_data.dump();
    config_pub_->publish(msg);
}

void ModbusDataProcessor::modbusControlCallback(const std_msgs::msg::String::SharedPtr msg) {
    try {
        // 解析JSON控制命令
        json control_data = json::parse(msg->data);
        
        bool has_operations = false;  // 标记是否有实际操作
        
        // 处理数字输出控制
        if (control_data.contains("digital_outputs")) {
            auto digital_outputs = control_data["digital_outputs"];
            for (auto& [key, value] : digital_outputs.items()) {
                int address = std::stoi(key);
                bool state = value.get<bool>();
                
                // 调用Modbus客户端设置线圈
                if (modbus_client_.writeSingleCoil(address, state)) {
                    has_operations = true;
                } else {
                    DEBUG_MODBUS_ERROR("Failed to set DO" << address << " to " << (state ? "ON" : "OFF"));
                }
            }
        }
        
        // 处理模拟输出控制
        if (control_data.contains("analog_outputs")) {
            auto analog_outputs = control_data["analog_outputs"];
            for (auto& [key, value] : analog_outputs.items()) {
                int address = std::stoi(key);
                uint16_t register_value = value.get<uint16_t>();
                
                // 调用Modbus客户端设置保持寄存器
                if (modbus_client_.writeSingleRegister(address, register_value)) {
                    has_operations = true;
                } else {
                    DEBUG_MODBUS_ERROR("Failed to set analog output register " << address << " to " << register_value);
                }
            }
        }
        
        // 处理批量操作
        if (control_data.contains("batch_operations")) {
            auto batch_ops = control_data["batch_operations"];
            
            if (batch_ops.contains("reset_all_analog")) {
                for (int i = 0; i < 4; i++) {
                    modbus_client_.writeSingleRegister(i, 0);
                }
                has_operations = true;
            }
            
            if (batch_ops.contains("turn_off_all_digital")) {
                for (int i = 0; i < 8; i++) {
                    modbus_client_.writeSingleCoil(i, false);
                }
                has_operations = true;
            }
        }
        
        // 只在有实际操作时才发布控制响应消息
        if (has_operations) {
            publishControlResponse(control_data, true);
        }
        
    } catch (const std::exception& e) {
        DEBUG_CORE_ERROR("Error parsing Modbus control command: " << e.what());
        
        // 发布错误响应
        json error_response;
        error_response["command"] = "error";
        error_response["error"] = e.what();
        error_response["timestamp"] = std::time(nullptr);
        error_response["success"] = false;
        publishControlResponse(error_response, false);
    }
}

void ModbusDataProcessor::publishControlResponse(const json& control_data, bool success) {
#ifdef ENABLE_ROS2
    if (control_response_pub_) {
        // 只在真正需要时才发布控制响应（避免频繁的广播日志）
        // 检查是否有实际的控制操作或错误
        bool should_publish = false;
        
        // 如果有错误，总是发布
        if (!success) {
            should_publish = true;
        }
        // 如果有实际的控制操作，发布响应
        else if (control_data.contains("digital_outputs") || 
                 control_data.contains("analog_outputs") || 
                 control_data.contains("batch_operations")) {
            should_publish = true;
        }
        
        if (should_publish) {
            json response;
            response["command"] = control_data.contains("command") ? control_data["command"] : "modbus_control";
            response["data"] = control_data;
            response["timestamp"] = std::time(nullptr);
            response["success"] = success;
            
            std_msgs::msg::String msg;
            msg.data = response.dump();
            control_response_pub_->publish(msg);
        }
    }
#endif
}
#endif

