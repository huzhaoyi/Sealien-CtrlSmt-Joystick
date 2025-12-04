/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-09-08 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-09-08 11:29:37
 * @FilePath: include/core/modbus_data_processor.h
 * @Description: Modbus数据处理器头文件
 * 负责处理Modbus数据的读取、解析和转换
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */
#pragma once

#include "config.h"
#include "modbus_client.h"
#include "uinput_device.h"
#include "utils/byte_order.h"
#include "utils/filters.h"
#include "utils/mapping.h"
#include "constants.h"

#ifdef ENABLE_ROS2
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/string.hpp>
#endif

#include <map>
#include <set>
#include <vector>
#include <cstdint>
#include <mutex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @brief 寄存器地址范围结构
 */
struct AddressRanges {
    int in_min{0}, in_max{-1};         ///< 输入寄存器范围
    int ho_min{0}, ho_max{-1};         ///< 保持寄存器范围
    int co_min{0}, co_max{-1};         ///< 线圈范围
};

/**
 * @brief Modbus数据处理器类
 * 
 * 该类负责：
 * - 分析配置并确定需要读取的寄存器地址范围
 * - 批量读取Modbus数据
 * - 处理轴数据转换和滤波
 * - 处理按钮数据转换和防抖
 * - 处理离散轴数据
 */
class ModbusDataProcessor {
public:
    /**
     * @brief 构造函数
     * @param config 配置对象
     * @param modbus_client Modbus客户端
     * @param uinput_device uinput设备（可选，如果为nullptr则仅使用ROS2输出）
     * @param ros2_node ROS2节点（可选）
     */
    ModbusDataProcessor(Config& config, ModbusClient& modbus_client, UInputDevice* uinput_device, 
                       rclcpp::Node::SharedPtr ros2_node = nullptr);

    /**
     * @brief 析构函数
     */
    ~ModbusDataProcessor() = default;

    /**
     * @brief 初始化处理器
     * @return 初始化成功返回true
     */
    bool initialize();

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
     * @brief 处理一轮Modbus数据
     * @return 处理成功返回true
     */
    bool processData();

    /**
     * @brief 清零所有输出
     */
    void clearAllOutputs();

    /**
     * @brief 检查处理器状态
     * @return 初始化成功返回true
     */
    bool isInitialized() const { return initialized_; }

    /**
     * @brief 获取地址范围信息
     * @return 地址范围结构
     */
    const AddressRanges& getAddressRanges() const { return ranges_; }

private:
    /**
     * @brief 收集配置中需要的寄存器地址集合
     */
    void collectAddressSets();

    /**
     * @brief 计算各类型寄存器的地址范围
     */
    void computeRanges();

    /**
     * @brief 批量读取所有Modbus数据
     * @param in_map 输入寄存器映射
     * @param ho_map 保持寄存器映射
     * @param co_map 线圈映射
     * @param di_map 离散量输入映射
     * @param update_bus_error 是否更新总线错误状态（多从站模式下应设为false，由外层统一处理）
     * @return 读取成功返回true
     */
    bool readAllData(std::map<int, uint16_t>& in_map,
                     std::map<int, uint16_t>& ho_map,
                     std::map<int, uint8_t>& co_map,
                     std::map<int, uint8_t>& di_map,
                     bool update_bus_error = true);

    /**
     * @brief 处理连续轴数据
     * @param in_map 输入寄存器映射
     * @param ho_map 保持寄存器映射
     */
    void processAxes(const std::map<int, uint16_t>& in_map,
                     const std::map<int, uint16_t>& ho_map);

    /**
     * @brief 处理多从站连续轴数据
     * @param slave_in_maps 每个从站的输入寄存器映射（从站ID -> 地址 -> 值）
     * @param slave_ho_maps 每个从站的保持寄存器映射（从站ID -> 地址 -> 值）
     */
    void processAxesMultiSlave(const std::map<int, std::map<int, uint16_t> >& slave_in_maps,
                                const std::map<int, std::map<int, uint16_t> >& slave_ho_maps);

    /**
     * @brief 处理离散轴数据
     * @param in_map 输入寄存器映射
     * @param ho_map 保持寄存器映射
     * @param co_map 线圈映射
     * @param di_map 离散量输入映射
     */
    void processDiscreteAxes(const std::map<int, uint16_t>& in_map,
                            const std::map<int, uint16_t>& ho_map,
                            const std::map<int, uint8_t>& co_map,
                            const std::map<int, uint8_t>& di_map);

    /**
     * @brief 处理按钮数据
     * @param in_map 输入寄存器映射
     * @param ho_map 保持寄存器映射
     * @param co_map 线圈映射
     * @param di_map 离散量输入映射
     */
    void processButtons(const std::map<int, uint16_t>& in_map,
                        const std::map<int, uint16_t>& ho_map,
                        const std::map<int, uint8_t>& co_map,
                        const std::map<int, uint8_t>& di_map);

    /**
     * @brief 计算集合中地址的最小值和最大值
     * @tparam T 数据类型
     * @param s 地址集合
     * @return 包含最小值和最大值的pair
     */
    template<typename T>
    std::pair<int, int> minmaxAddr(const std::set<int>& s);

    /**
     * @brief 将电压值转换为Modbus原始值
     * @param voltage 电压值 (V)
     * @return Modbus原始值 (0-4095)
     */
    uint16_t voltageToModbusValue(double voltage);

    /**
     * @brief 根据电压映射参数计算Modbus值范围
     * @return 包含min和max的pair
     */
    std::pair<uint16_t, uint16_t> calculateModbusRange();

#ifdef ENABLE_ROS2
    /**
     * @brief 发布Modbus设备ROS2消息
     */
    void publishModbusROS2Messages();
    
    /**
     * @brief 发布原始Modbus数据到ROS2话题
     */
    void publishRawModbusData();
    
    /**
     * @brief 更新原始Modbus数据
     * @param in_map 输入寄存器映射
     * @param ho_map 保持寄存器映射
     * @param co_map 线圈映射
     * @param di_map 离散量输入映射
     * @param connection_status 连接状态
     */
    void updateRawModbusData(const std::map<int, uint16_t>& in_map,
                            const std::map<int, uint16_t>& ho_map,
                            const std::map<int, uint8_t>& co_map,
                            const std::map<int, uint8_t>& di_map,
                            bool connection_status);
    
    /**
     * @brief 发布配置信息到ROS2话题
     */
    void publishConfigData();
    
    /**
     * @brief Modbus控制命令回调函数
     * @param msg 控制命令JSON字符串
     */
    void modbusControlCallback(const std_msgs::msg::String::SharedPtr msg);
    void publishControlResponse(const json& control_data, bool success);
#endif

    // 引用对象
    Config& config_;                    ///< 配置对象引用
    ModbusClient& modbus_client_;       ///< Modbus客户端引用
    UInputDevice* uinput_device_;       ///< uinput设备指针（可选，nullptr时仅使用ROS2输出）

    // 地址集合
    std::set<int> need_in_regs_;        ///< 需要的输入寄存器地址
    std::set<int> need_hold_regs_;      ///< 需要的保持寄存器地址
    std::set<int> need_coils_;          ///< 需要的线圈地址
    std::set<int> need_discrete_inputs_; ///< 需要的离散量输入地址

    // 地址范围
    AddressRanges ranges_;              ///< 地址范围

#ifdef ENABLE_ROS2
    // ROS2相关
    rclcpp::Node::SharedPtr ros2_node_; ///< ROS2节点
    rclcpp::Publisher<sensor_msgs::msg::Joy>::SharedPtr joy_pub_;           ///< 手柄数据发布者
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr twist_pub_;     ///< 速度命令发布者
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_pub_;        ///< 设备状态发布者
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr modbus_raw_pub_;   ///< 原始Modbus数据发布者
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr config_pub_;        ///< 配置信息发布者
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr control_sub_;     ///< Modbus控制订阅者
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr control_response_pub_; ///< Modbus控制响应发布者
    
    // Modbus设备数据存储
    struct ModbusDeviceData {
        std::vector<float> axes;            ///< 轴数据，范围[-1.0, 1.0]
        std::vector<int> buttons;           ///< 按钮数据，0=释放，1=按下
        rclcpp::Time last_update;           ///< 最后更新时间
    } modbus_device_data_;                  ///< Modbus设备数据
    std::mutex modbus_data_mutex_;          ///< Modbus数据互斥锁
    std::string modbus_device_path_;        ///< Modbus设备路径标识
    
    // 原始Modbus数据存储
    struct RawModbusData {
        std::map<int, uint16_t> input_registers;     ///< 输入寄存器原始数据
        std::map<int, uint16_t> holding_registers;    ///< 保持寄存器原始数据
        std::map<int, uint8_t> coils;                ///< 线圈原始数据
        std::map<int, uint8_t> discrete_inputs;      ///< 离散量输入原始数据
        std::map<int, double> voltages;              ///< 电压值（从寄存器转换）
        rclcpp::Time last_update;                     ///< 最后更新时间
        bool connection_status;                       ///< 连接状态
    } raw_modbus_data_;                               ///< 原始Modbus数据
    std::mutex raw_modbus_data_mutex_;               ///< 原始Modbus数据互斥锁
#endif

    // 状态标志
    bool initialized_{false};           ///< 初始化状态
    
    // 轴映射配置
    struct AxisMapping {
        int linear_x{-1};    ///< 线速度X轴索引
        int linear_y{-1};    ///< 线速度Y轴索引
        int linear_z{-1};    ///< 线速度Z轴索引
        int angular_x{-1};   ///< 角速度X轴索引
        int angular_y{-1};   ///< 角速度Y轴索引
        int angular_z{-1};   ///< 角速度Z轴索引
    } axis_mapping_;         ///< 轴映射配置
};
