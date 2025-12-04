/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-09-08 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-09-08 11:29:37
 * @FilePath: include/core/joystick_manager.h
 * @Description: USB手柄管理器头文件
 * 负责管理USB物理手柄的检测、事件处理和ROS2消息发布
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */
#pragma once

#include "detection/joystick_detector.h"
#include "constants.h"

#ifdef ENABLE_ROS2
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <memory>
#include <mutex>
#include <map>
#include <vector>
#endif

#include <functional>
#include <atomic>
#include <chrono>
#include <thread>

/**
 * @brief USB手柄管理器类
 * 
 * 该类负责：
 * - 管理USB物理手柄的检测和连接状态
 * - 处理手柄输入事件
 * - 发布ROS2消息（如果启用）
 * - 提供手柄状态查询接口
 */
class JoystickManager {
public:
    /**
     * @brief 事件回调函数类型
     */
    using EventCallback = std::function<void(const std::string& device_path, 
                                           int event_type, 
                                           int code, 
                                           int value)>;

    /**
     * @brief 构造函数
     */
    JoystickManager();

    /**
     * @brief 析构函数
     */
    ~JoystickManager();

    /**
     * @brief 初始化管理器
     * @param enable_ros2 是否启用ROS2功能
     * @param ros2_node ROS2节点指针（如果启用ROS2）
     * @return 初始化成功返回true
     */
    bool initialize(bool enable_ros2 = false, 
#ifdef ENABLE_ROS2
                   rclcpp::Node::SharedPtr ros2_node = nullptr
#else
                   void* ros2_node = nullptr
#endif
                   );

    /**
     * @brief 设置轴映射配置
     * @param linear_x_axis 线速度X轴对应的轴索引
     * @param linear_y_axis 线速度Y轴对应的轴索引
     * @param linear_z_axis 线速度Z轴对应的轴索引
     * @param angular_x_axis 角速度X轴对应的轴索引
     * @param angular_y_axis 角速度Y轴对应的轴索引
     * @param angular_z_axis 角速度Z轴对应的轴索引
     */
    void setAxisMapping(int linear_x_axis = -1, int linear_y_axis = -1, int linear_z_axis = -1,
                       int angular_x_axis = -1, int angular_y_axis = -1, int angular_z_axis = -1);

    /**
     * @brief 启动手柄检测器
     * @return 启动成功返回true
     */
    bool start();

    /**
     * @brief 停止手柄检测器
     */
    void stop();

    /**
     * @brief 设置事件回调函数
     * @param callback 回调函数
     */
    void setEventCallback(EventCallback callback);

    /**
     * @brief 获取已连接的手柄数量
     * @return 手柄数量
     */
    int getConnectedDeviceCount() const;

    /**
     * @brief 获取所有已连接手柄的信息
     * @return 手柄信息列表
     */
    std::vector<PhysicalJoystickInfo> getConnectedDevices() const;

    /**
     * @brief 检查管理器是否正在运行
     * @return 正在运行返回true
     */
    bool isRunning() const;

    /**
     * @brief 检查管理器是否已初始化
     * @return 已初始化返回true
     */
    bool isInitialized() const { return initialized_; }

    /**
     * @brief 暂停USB手柄数据处理（用于模式切换）
     */
    void pause();

    /**
     * @brief 恢复USB手柄数据处理（用于模式切换）
     */
    void resume();

    /**
     * @brief 检查是否处于暂停状态
     * @return 暂停状态返回true
     */
    bool isPaused() const { return paused_.load(); }

    /**
     * @brief 检查设备是否应该被处理
     * @param device_path 设备路径
     * @return 应该处理返回true
     */
    bool shouldProcessDevice(const std::string& device_path) const;

    /**
     * @brief 设置定时发送间隔
     * @param interval_ms 发送间隔（毫秒）
     */
    void setTimerInterval(int interval_ms);

    /**
     * @brief 获取当前定时发送间隔
     * @return 发送间隔（毫秒）
     */
    int getTimerInterval() const { return timer_interval_ms_; }

#ifdef ENABLE_ROS2
    /**
     * @brief 发布设备状态消息
     * @param status 状态字符串
     * @param device_path 设备路径
     */
    void publishDeviceStatus(const std::string& status, const std::string& device_path);
#endif

private:
#ifdef ENABLE_ROS2
    /**
     * @brief 初始化ROS2发布者
     */
    void initializeROSPublishers();

    /**
     * @brief 处理手柄连接事件
     * @param device_path 设备路径
     */
    void handleDeviceConnect(const std::string& device_path);

    /**
     * @brief 处理手柄断开事件
     * @param device_path 设备路径
     */
    void handleDeviceDisconnect(const std::string& device_path);

    /**
     * @brief 处理轴事件
     * @param device_path 设备路径
     * @param axis 轴索引
     * @param value 轴值
     */
    void handleAxisEvent(const std::string& device_path, int axis, int value);

    /**
     * @brief 处理按钮事件
     * @param device_path 设备路径
     * @param button 按钮索引
     * @param value 按钮值
     */
    void handleButtonEvent(const std::string& device_path, int button, int value);

    /**
     * @brief 发布Joy消息
     * @param device_path 设备路径
     */
    void publishJoyMessage(const std::string& device_path);

    /**
     * @brief 发布Twist消息
     * @param device_path 设备路径
     */
    void publishTwistMessage(const std::string& device_path);

    /**
     * @brief 定时发送线程函数
     */
    void timerThread();

    /**
     * @brief 读取设备当前状态（用于定时发送）
     * @param device_path 设备路径
     */
    void readDeviceCurrentState(const std::string& device_path);

    /**
     * @brief 更新轴数据（替代事件触发）
     * @param device_path 设备路径
     * @param axis 轴索引
     * @param value 轴值
     */
    void updateAxisData(const std::string& device_path, int axis, int value);

    /**
     * @brief 更新按钮数据（替代事件触发）
     * @param device_path 设备路径
     * @param button 按钮索引
     * @param value 按钮值
     */
    void updateButtonData(const std::string& device_path, int button, int value);

#endif

    /**
     * @brief 手柄事件回调函数
     * @param device_path 设备路径
     * @param event_type 事件类型
     * @param code 事件代码
     * @param value 事件值
     */
    void onJoystickEvent(const std::string& device_path, int event_type, int code, int value);

    // 核心组件
    std::unique_ptr<JoystickDetector> joystick_detector_; ///< 手柄检测器

    // 状态标志
    bool initialized_{false};           ///< 初始化状态
    bool enable_ros2_{false};            ///< 是否启用ROS2
    std::atomic<bool> paused_{false};    ///< 暂停状态
    
    // 轴映射配置
    struct AxisMapping {
        int linear_x{-1};    ///< 线速度X轴索引
        int linear_y{-1};    ///< 线速度Y轴索引
        int linear_z{-1};    ///< 线速度Z轴索引
        int angular_x{-1};   ///< 角速度X轴索引
        int angular_y{-1};   ///< 角速度Y轴索引
        int angular_z{-1};   ///< 角速度Z轴索引
    } axis_mapping_;         ///< 轴映射配置
    
    // 性能优化相关
    std::chrono::steady_clock::time_point last_publish_time_;  ///< 上次发布消息的时间
    static constexpr std::chrono::milliseconds PUBLISH_INTERVAL{16};  ///< 发布间隔（约60Hz）
    
    // 定时发送相关
    std::atomic<bool> timer_running_{false};     ///< 定时器运行状态
    std::thread timer_thread_;                   ///< 定时发送线程
    std::atomic<int> timer_interval_ms_;  ///< 定时发送间隔

#ifdef ENABLE_ROS2
    // ROS2相关
    rclcpp::Node::SharedPtr ros2_node_; ///< ROS2节点指针
    rclcpp::Publisher<sensor_msgs::msg::Joy>::SharedPtr joy_pub_;           ///< 手柄数据发布者
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr twist_pub_;     ///< 速度命令发布者
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_pub_;       ///< 设备状态发布者

    // 数据缓存
    std::map<std::string, std::vector<float>> device_axes_;     ///< 设备轴数据缓存
    std::map<std::string, std::vector<int>> device_buttons_;    ///< 设备按钮数据缓存
    std::mutex device_data_mutex_;                              ///< 设备数据互斥锁
    
    // 设备状态缓存（减少锁竞争）
    struct DeviceState {
        std::vector<float> axes;
        std::vector<int> buttons;
        std::mutex mutex;
        std::chrono::steady_clock::time_point last_update;
    };
    std::map<std::string, std::unique_ptr<DeviceState>> device_states_;  ///< 设备状态缓存
#endif

    // 事件回调
    EventCallback event_callback_;       ///< 外部事件回调函数
};
