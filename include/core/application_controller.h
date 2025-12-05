/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-09-08 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-09-08 11:29:37
 * @FilePath: include/core/application_controller.h
 * @Description: 应用程序主控制器头文件
 * 负责协调各个模块，管理应用程序的生命周期
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */
#pragma once

#include "config.h"
#include "config/smart_config.h"
#include "communication/modbus/modbus_client.h"
#include "input/uinput_device.h"
#include "communication/modbus/acquisition_card_processor.h"
#include "communication/modbus/valve_control_processor.h"
#include "communication/joystick/joystick_manager.h"
#include "detection/usb_serial_detector.h"
#include "constants.h"

#ifdef ENABLE_ROS2
#include <rclcpp/rclcpp.hpp>
#include <memory>
#endif

#include <string>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>

/**
 * @brief 应用程序主控制器类
 * 
 * 该类负责：
 * - 协调各个模块的初始化和运行
 * - 管理应用程序的生命周期
 * - 处理信号和优雅退出
 * - 根据配置选择运行模式（Modbus模式或纯USB手柄模式）
 */
class ApplicationController {
public:
    /**
     * @brief 构造函数
     */
    ApplicationController();

    /**
     * @brief 析构函数
     */
    ~ApplicationController();

    /**
     * @brief 初始化应用程序
     * @param config_path 配置文件路径（空字符串表示使用默认配置）
     * @param enable_ros2 是否启用ROS2功能
     * @return 初始化成功返回true
     */
    bool initialize(const std::string& config_path, bool enable_ros2 = false);

    /**
     * @brief 运行应用程序主循环
     * @return 运行成功返回true
     */
    bool run();

    /**
     * @brief 停止应用程序
     */
    void stop();

    /**
     * @brief 检查应用程序是否正在运行
     * @return 正在运行返回true
     */
    bool isRunning() const { return g_run.load(); }

    /**
     * @brief 检查应用程序是否已初始化
     * @return 已初始化返回true
     */
    bool isInitialized() const { return initialized_; }

    /**
     * @brief 获取运行模式
     * @return 运行模式字符串
     */
    std::string getRunMode() const;

private:
    /**
     * @brief 初始化ROS2
     * @param argc 命令行参数数量
     * @param argv 命令行参数数组
     * @return 初始化成功返回true
     */
    bool initializeROS2(int argc, char** argv);

    /**
     * @brief 初始化配置
     * @param config_path 配置文件路径
     * @return 初始化成功返回true
     */
    bool initializeConfig(const std::string& config_path);

    /**
     * @brief 初始化Modbus客户端
     * @return 初始化成功返回true
     */
    bool initializeModbus();

    /**
     * @brief 初始化uinput设备
     * @return 初始化成功返回true
     */
    bool initializeUInput();

    /**
     * @brief 初始化采集卡数据处理器
     * @return 初始化成功返回true
     */
    bool initializeAcquisitionCardProcessor();

    /**
     * @brief 初始化阀控板处理器
     * @return 初始化成功返回true
     */
    bool initializeValveControlProcessor();

    /**
     * @brief 初始化手柄管理器
     * @return 初始化成功返回true
     */
    bool initializeJoystickManager();

    /**
     * @brief 初始化USB转串口设备检测器
     * @return 初始化成功返回true
     */
    bool initializeUSBSerialDetector();

    /**
     * @brief 注册信号处理器
     */
    void registerSignalHandlers();

    /**
     * @brief 信号处理函数
     * @param sig 信号编号
     */
    static void onSignal(int sig);

    /**
     * @brief 运行Modbus模式主循环
     */
    void runModbusMode();

    /**
     * @brief 运行纯USB手柄模式主循环
     */
    void runUSBOnlyMode();

    /**
     * @brief 运行动态检测模式（热插拔单一模式切换）
     */
    void runDynamicMode();

    /**
     * @brief 清理指定模式的资源
     * @param mode 模式名称 ("modbus", "usb", "standby")
     */
    void cleanupCurrentMode(const std::string& mode);

    /**
     * @brief 初始化指定模式
     * @param mode 模式名称 ("modbus", "usb", "standby")
     * @return 初始化成功返回true
     */
    bool initializeMode(const std::string& mode);

    /**
     * @brief 清理资源
     */
    void cleanup();

    /**
     * @brief 发布系统心跳状态
     */
    void publishSystemHeartbeat();

    // 全局运行标志（原子），用于信号处理与多线程安全退出
    static std::atomic<bool> g_run;

    // 配置和核心组件
    std::unique_ptr<SmartConfigManager> smart_config_;   ///< 智能配置管理器
    std::unique_ptr<ModbusClient> modbus_client_;     ///< Modbus客户端
    std::unique_ptr<UInputDevice> uinput_device_;     ///< uinput设备
    std::unique_ptr<AcquisitionCardProcessor> acquisition_card_processor_; ///< 采集卡数据处理器
    std::unique_ptr<ValveControlProcessor> valve_control_processor_; ///< 阀控板处理器
    std::unique_ptr<JoystickManager> joystick_manager_; ///< 手柄管理器
    std::unique_ptr<USBSerialDetector> usb_serial_detector_; ///< USB转串口设备检测器

#ifdef ENABLE_ROS2
    // ROS2相关
    rclcpp::Node::SharedPtr ros2_node_;              ///< ROS2节点
    bool ros2_initialized_{false};                    ///< ROS2初始化状态
#endif

    // 状态标志
    bool initialized_{false};                         ///< 初始化状态
    bool enable_ros2_{false};                          ///< 是否启用ROS2
    bool enable_modbus_{false};                        ///< 是否启用Modbus
    bool modbus_connected_{false};                     ///< Modbus连接状态
    bool uinput_created_{false};                      ///< uinput设备创建状态

    // 运行参数
    std::string config_path_;                          ///< 配置文件路径
    double poll_hz_{10.0};                            ///< 轮询频率
    std::string valve_control_port_;                   ///< 阀控板串口路径
    std::string acquisition_card_serial_number_;       ///< 采集卡 USB 序列号（用于重连识别）
    std::string valve_control_serial_number_;          ///< 阀控板设备 USB 序列号（用于重连识别）

    /**
     * @brief 解析符号链接到实际设备路径
     * @param path 设备路径（可能是符号链接）
     * @return 实际设备路径，如果解析失败则返回原路径
     */
    std::string resolveSymbolicLink(const std::string& path) const;

    /**
     * @brief 从设备路径获取 USB 序列号
     * @param device_path 设备路径
     * @return USB 序列号，如果获取失败则返回空字符串
     */
    std::string getUSBSerialNumber(const std::string& device_path) const;

    /**
     * @brief 通过 USB 序列号查找设备路径
     * @param serial_number USB 序列号
     * @param exclude_ports 要排除的设备路径列表（避免冲突）
     * @return 设备路径，如果未找到则返回空字符串
     */
    std::string findDeviceBySerialNumber(const std::string& serial_number, const std::vector<std::string>& exclude_ports = {}) const;
};
