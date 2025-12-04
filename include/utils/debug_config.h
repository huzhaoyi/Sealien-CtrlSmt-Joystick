/*
 * @Author: Auto Generated
 * @Date: 2025-01-XX
 * @Description: 调试配置管理器 - 运行时控制调试开关
 * 支持通过配置文件动态控制各模块的调试输出
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#ifndef DEBUG_CONFIG_H
#define DEBUG_CONFIG_H

#include <string>
#include <map>

/**
 * @brief 调试配置结构体
 * 包含所有模块的调试开关
 */
struct DebugConfig {
    // Core模块调试开关
    bool debug_core = true;
    bool debug_config = true;
    
    // Modbus分级调试开关
    bool debug_modbus_comm = true;          // Modbus基础通信调试
    bool debug_modbus_input_reg = false;    // Modbus输入寄存器调试
    bool debug_modbus_holding_reg = false; // Modbus保持寄存器调试
    bool debug_modbus_coil = false;         // Modbus线圈调试
    bool debug_modbus_discrete_input = false; // Modbus离散输入调试
    bool debug_modbus_voltage_conv = false; // Modbus电压转换调试
    bool debug_modbus_write = false;        // Modbus写入操作调试
    bool debug_modbus_hex_frame = false;    // Modbus十六进制数据帧打印
    
    // Joystick模块调试开关
    bool debug_joystick = true;
    
    // ROS2分级调试开关
    bool debug_ros2_comm = true;            // ROS2基础通信调试
    bool debug_ros2_axes = false;           // ROS2轴数据调试
    bool debug_ros2_buttons = false;        // ROS2按钮数据调试
    bool debug_ros2_twist = false;          // ROS2 Twist消息调试
    bool debug_ros2_status = false;        // ROS2状态消息调试
    bool debug_ros2_publish = false;       // ROS2消息发布调试
    bool debug_ros2_subscribe = false;     // ROS2订阅调试
    
    // Utils模块调试开关
    bool debug_utils = true;
    
    // Mapping模块调试开关
    bool debug_mapping = true;
    
    // UInput模块调试开关
    bool debug_uinput = true;
    
    // 阀控板模块调试开关
    bool debug_valve_control = true;        // 阀控板基础调试
    bool debug_valve_control_status = false; // 阀控板状态读取调试（总开关）
    bool debug_valve_control_status_registers = false; // 所有寄存器原始数据
    bool debug_valve_control_status_key = false; // 关键寄存器
    bool debug_valve_control_status_current = false; // 阀电流值
    bool debug_valve_control_status_sensor = false; // 传感器数据
    bool debug_valve_control_status_encoder = false; // 编码器数据
    bool debug_valve_control_status_proximity = false; // 接近开关
    bool debug_valve_control_status_env = false; // 湿度/温度/板载传感器
    bool debug_valve_control_status_sensor_status = false; // 传感器状态
    bool debug_valve_control_status_valve_status = false; // 阀状态
    bool debug_valve_control_write = false;  // 阀控板写入操作调试
    bool debug_valve_control_joystick = false; // 阀控板摇杆消息调试
    
    /**
     * @brief 从配置映射设置调试开关
     * @param config_map 配置映射表（键值对）
     */
    void fromMap(const std::map<std::string, bool>& config_map);
    
    /**
     * @brief 获取所有调试开关的映射表
     * @return 配置映射表
     */
    std::map<std::string, bool> toMap() const;
    
    /**
     * @brief 设置所有调试开关为默认值
     */
    void setDefaults();
    
    /**
     * @brief 启用所有调试开关
     */
    void enableAll();
    
    /**
     * @brief 禁用所有调试开关
     */
    void disableAll();
};

/**
 * @brief 调试配置管理器（单例模式）
 * 提供全局访问点，用于运行时控制调试开关
 */
class DebugConfigManager {
public:
    /**
     * @brief 获取单例实例
     * @return DebugConfigManager实例的引用
     */
    static DebugConfigManager& getInstance();
    
    /**
     * @brief 从DebugConfig结构体应用配置
     * @param config 调试配置
     */
    void applyConfig(const DebugConfig& config);
    
    /**
     * @brief 从YAML文件加载调试配置
     * @param config_path 配置文件路径（如果为空，尝试从默认路径加载）
     * @return 是否成功加载
     */
    bool loadFromFile(const std::string& config_path = "");
    
    /**
     * @brief 获取当前调试配置
     * @return 当前调试配置的引用
     */
    const DebugConfig& getConfig() const { return config_; }
    
    // 各个模块的调试开关检查函数
    bool isDebugCore() const { return config_.debug_core; }
    bool isDebugConfig() const { return config_.debug_config; }
    
    bool isDebugModbusComm() const { return config_.debug_modbus_comm; }
    bool isDebugModbusInputReg() const { return config_.debug_modbus_input_reg; }
    bool isDebugModbusHoldingReg() const { return config_.debug_modbus_holding_reg; }
    bool isDebugModbusCoil() const { return config_.debug_modbus_coil; }
    bool isDebugModbusDiscreteInput() const { return config_.debug_modbus_discrete_input; }
    bool isDebugModbusVoltageConv() const { return config_.debug_modbus_voltage_conv; }
    bool isDebugModbusWrite() const { return config_.debug_modbus_write; }
    bool isDebugModbusHexFrame() const { return config_.debug_modbus_hex_frame; }
    
    bool isDebugJoystick() const { return config_.debug_joystick; }
    
    bool isDebugROS2Comm() const { return config_.debug_ros2_comm; }
    bool isDebugROS2Axes() const { return config_.debug_ros2_axes; }
    bool isDebugROS2Buttons() const { return config_.debug_ros2_buttons; }
    bool isDebugROS2Twist() const { return config_.debug_ros2_twist; }
    bool isDebugROS2Status() const { return config_.debug_ros2_status; }
    bool isDebugROS2Publish() const { return config_.debug_ros2_publish; }
    bool isDebugROS2Subscribe() const { return config_.debug_ros2_subscribe; }
    
    bool isDebugUtils() const { return config_.debug_utils; }
    bool isDebugMapping() const { return config_.debug_mapping; }
    bool isDebugUInput() const { return config_.debug_uinput; }
    
    bool isDebugValveControl() const { return config_.debug_valve_control; }
    bool isDebugValveControlStatus() const { return config_.debug_valve_control_status; }
    bool isDebugValveControlStatusRegisters() const { return config_.debug_valve_control_status && config_.debug_valve_control_status_registers; }
    bool isDebugValveControlStatusKey() const { return config_.debug_valve_control_status && config_.debug_valve_control_status_key; }
    bool isDebugValveControlStatusCurrent() const { return config_.debug_valve_control_status && config_.debug_valve_control_status_current; }
    bool isDebugValveControlStatusSensor() const { return config_.debug_valve_control_status && config_.debug_valve_control_status_sensor; }
    bool isDebugValveControlStatusEncoder() const { return config_.debug_valve_control_status && config_.debug_valve_control_status_encoder; }
    bool isDebugValveControlStatusProximity() const { return config_.debug_valve_control_status && config_.debug_valve_control_status_proximity; }
    bool isDebugValveControlStatusEnv() const { return config_.debug_valve_control_status && config_.debug_valve_control_status_env; }
    bool isDebugValveControlStatusSensorStatus() const { return config_.debug_valve_control_status && config_.debug_valve_control_status_sensor_status; }
    bool isDebugValveControlStatusValveStatus() const { return config_.debug_valve_control_status && config_.debug_valve_control_status_valve_status; }
    bool isDebugValveControlWrite() const { return config_.debug_valve_control_write; }
    bool isDebugValveControlJoystick() const { return config_.debug_valve_control_joystick; }
    
private:
    DebugConfigManager() = default;
    ~DebugConfigManager() = default;
    DebugConfigManager(const DebugConfigManager&) = delete;
    DebugConfigManager& operator=(const DebugConfigManager&) = delete;
    
    DebugConfig config_;  // 当前调试配置
};

#endif // DEBUG_CONFIG_H

