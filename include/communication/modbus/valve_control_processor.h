/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-01-XX XX:XX:XX
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-01-XX XX:XX:XX
 * @FilePath: include/communication/modbus/valve_control_processor.h
 * @Description: 阀控板Modbus处理器头文件
 * 负责处理阀控板的Modbus通信，使用功能码03读取保持寄存器
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#pragma once

#include "modbus_client.h"
#include "constants.h"
#include <vector>
#include <map>
#include <cstdint>
#include <chrono>
#include <atomic>
#include <mutex>
#include <string>
#include <memory>

#ifdef ENABLE_ROS2
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <memory>
#endif

/**
 * @brief 阀控板状态结构
 */
struct ValveControlStatus {
    uint16_t accumulator;              ///< 累加寄存器（地址0），每1s+1，用于判断故障
    std::vector<uint16_t> valve_status; ///< 阀状态（地址56-60），每2bit表示1个阀的状态
    uint16_t heartbeat;                 ///< 主机心跳设置值（地址63）
    bool valid;                         ///< 数据是否有效
    std::chrono::steady_clock::time_point last_update; ///< 最后更新时间
};

/**
 * @brief 阀控板Modbus处理器类
 * 
 * 该类负责：
 * - 使用功能码03（读保持寄存器）轮询读取阀控板状态
 * - 将摇杆值映射到阀电流设定值并写入保持寄存器（地址64开始）
 */
class ValveControlProcessor {
public:
    /**
     * @brief 构造函数（使用独立的串口和波特率）
     * @param port 串口设备路径（如/dev/ttyUSB1）
     * @param baud 波特率
     * @param parity 校验位（N-无校验，E-偶校验，O-奇校验）
     * @param data_bits 数据位
     * @param stop_bits 停止位
     * @param slave_id 从站ID
     * @param poll_interval_ms 轮询间隔（毫秒），默认100ms
     */
    ValveControlProcessor(const std::string& port, int baud, char parity, int data_bits, int stop_bits, int slave_id, int poll_interval_ms = 100);

    /**
     * @brief 析构函数
     */
    ~ValveControlProcessor() = default;

    /**
     * @brief 初始化处理器
     * @return 初始化成功返回true
     */
    bool initialize();

    /**
     * @brief 轮询读取阀控板状态
     * @return 读取成功返回true
     */
    bool pollValveStatus();

    /**
     * @brief 获取阀控板状态
     * @return 阀控板状态结构
     */
    ValveControlStatus getStatus() const;

    /**
     * @brief 设置阀电流值（从摇杆值映射）
     * @param valve_index 阀索引（0-39，对应阀1-40）
     * @param joystick_value 摇杆值（-1.0到1.0）
     * @return 写入成功返回true
     */
    bool setValveCurrent(int valve_index, double joystick_value);

    /**
     * @brief 批量设置多个阀的电流值
     * @param joystick_values 摇杆值数组（-1.0到1.0），索引对应阀索引
     * @return 写入成功返回true
     */
    bool setValveCurrents(const std::vector<double>& joystick_values);

    /**
     * @brief 设置主机心跳值
     * @param heartbeat_value 心跳值（每次发送不同的心跳值）
     * @return 写入成功返回true
     */
    bool setHeartbeat(uint16_t heartbeat_value);

    /**
     * @brief 检查连接状态
     * @return 连接正常返回true
     */
    bool isConnected() const;

    /**
     * @brief 获取最后错误信息
     * @return 错误信息字符串
     */
    std::string getLastError() const;

    /**
     * @brief 触发重连（用于USB设备热插拔）
     * @return 重连成功返回true
     */
    bool reconnect();

    /**
     * @brief 关闭连接（用于USB设备断开）
     */
    void close();

#ifdef ENABLE_ROS2
    /**
     * @brief 设置ROS2节点（用于订阅摇杆话题）
     * @param ros2_node ROS2节点指针
     */
    void setROS2Node(rclcpp::Node::SharedPtr ros2_node);

    /**
     * @brief 从ROS2话题更新摇杆值并映射到阀电流
     * @param axes 摇杆轴值数组（-1.0到1.0）
     */
    void updateFromJoystickAxes(const std::vector<float>& axes);
#endif

private:
    std::unique_ptr<ModbusClient> modbus_client_;  ///< Modbus客户端（独立实例）
    int poll_interval_ms_;              ///< 轮询间隔（毫秒）
    ValveControlStatus status_;         ///< 阀控板状态
    mutable std::mutex status_mutex_;   ///< 状态互斥锁
    
    // 阀电流设定值缓存（避免重复写入相同值）
    std::vector<int16_t> last_valve_currents_; ///< 上次写入的阀电流值
    std::chrono::steady_clock::time_point last_poll_time_; ///< 上次轮询时间
    
    std::string last_error_;            ///< 最后错误信息
    
#ifdef ENABLE_ROS2
    rclcpp::Node::SharedPtr ros2_node_; ///< ROS2节点指针（用于订阅摇杆话题）
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_; ///< 摇杆话题订阅者
    std::vector<float> last_joystick_axes_; ///< 上次接收到的摇杆轴值
    std::mutex joystick_mutex_;         ///< 摇杆数据互斥锁
    
    /**
     * @brief ROS2摇杆话题回调函数
     * @param msg 摇杆消息
     */
    void joyCallback(const sensor_msgs::msg::Joy::SharedPtr msg);
#endif
    
    // 常量定义
    static constexpr int VALVE_COUNT = 40;           ///< 阀数量
    static constexpr int STATUS_REG_START = 56;      ///< 阀状态寄存器起始地址
    static constexpr int STATUS_REG_COUNT = 5;       ///< 阀状态寄存器数量（56-60）
    static constexpr int CURRENT_SET_START = 64;     ///< 阀电流设定起始地址
    static constexpr int HEARTBEAT_REG = 63;         ///< 心跳寄存器地址
    static constexpr int ACCUMULATOR_REG = 0;        ///< 累加寄存器地址
    
    // 电流值映射参数
    static constexpr int16_t CURRENT_MIN = 0;         ///< 最小电流值（对应0mA）
    static constexpr int16_t CURRENT_MAX = 30000;    ///< 最大电流值（对应1500mA）
    static constexpr double JOYSTICK_MIN = -1.0;     ///< 摇杆最小值
    static constexpr double JOYSTICK_MAX = 1.0;     ///< 摇杆最大值
    
    /**
     * @brief 将摇杆值转换为阀电流值
     * @param joystick_value 摇杆值（-1.0到1.0）
     * @return 阀电流值（0-30000，对应0mA-1500mA）
     */
    int16_t joystickToCurrent(double joystick_value) const;
    
    /**
     * @brief 解析阀状态寄存器
     * @param status_regs 状态寄存器值数组
     */
    void parseValveStatus(const std::vector<uint16_t>& status_regs);
    
    /**
     * @brief 打印所有寄存器的详细信息（按文档说明）
     * @param all_regs 所有寄存器值数组
     */
    void printAllRegistersDetailed(const std::vector<uint16_t>& all_regs) const;
};

