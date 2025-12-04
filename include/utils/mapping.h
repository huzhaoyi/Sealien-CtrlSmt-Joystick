/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:28:47
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:28:47
 * @FilePath: include/mapping.h
 * @Description: 配置映射和数据结构头文件
 * 定义Modbus摇杆配置的数据结构，包括轴映射、按钮映射和离散轴配置
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */
#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <linux/input.h>
#include "filters.h"
#include "constants.h"
#include "debug_config.h"

/**
 * @brief 轴数据源类型枚举
 *
 * 定义轴数据可以来自的Modbus寄存器类型
 */
enum class AxisSourceKind
{
  InputRegister,  ///< 输入寄存器（只读）
  HoldingRegister ///< 保持寄存器（可读写）
};

/**
 * @brief 按钮数据源类型枚举
 *
 * 定义按钮数据可以来自的Modbus数据类型
 */
enum class ButtonSourceKind
{
  Coil,              ///< 线圈（单个位）
  DiscreteInput,     ///< 离散量输入（功能码0x02）
  InputRegisterBit,  ///< 输入寄存器的指定位
  HoldingRegisterBit ///< 保持寄存器的指定位
};

/**
 * @brief 轴数据源结构
 *
 * 描述轴数据的来源，包括类型和地址
 */
struct AxisSource
{
  AxisSourceKind kind{AxisSourceKind::InputRegister}; ///< 数据源类型
  int addr{0};                                        ///< 寄存器地址
};

/**
 * @brief 寄存器位源结构
 *
 * 描述按钮或离散轴数据的来源，支持线圈和寄存器的位操作
 */
struct RegBitSource
{
  ButtonSourceKind kind{ButtonSourceKind::Coil}; ///< 数据源类型
  int addr{0};                                   ///< 线圈编号或寄存器地址
  int bit{INVALID_BIT_INDEX};                    ///< 位索引（0-15），-1表示整个寄存器
};

/**
 * @brief 连续轴映射配置
 *
 * 定义连续轴（如摇杆的X/Y轴）的配置参数
 */
struct AxisMap
{
  int code{ABS_X};        ///< 轴代码（如ABS_X, ABS_Y等）
  AxisSource src;         ///< 数据源配置
  int raw_min{DEFAULT_RAW_MIN};         ///< 原始值最小值
  int raw_max{DEFAULT_RAW_MAX};         ///< 原始值最大值
  double deadzone{DEFAULT_DEADZONE};    ///< 死区大小（0.0-0.49）
  double lpf_alpha{DEFAULT_LPF_ALPHA};  ///< 低通滤波系数（0.0-1.0）
  bool invert{DEFAULT_INVERT};          ///< 是否反转轴值
  bool swap_endian{false};              ///< 是否需要交换字节序
  int ros2_joy_index{-1};               ///< ROS2 Joy消息中的轴索引（-1表示未映射）
  LowPassFilter lpf{1.0};               ///< 低通滤波器实例
};

/**
 * @brief 按钮映射配置
 *
 * 定义按钮的配置参数，包括数据源和去抖设置
 */
struct ButtonMap
{
  int code{BTN_TRIGGER_HAPPY1}; ///< 按键代码
  RegBitSource src;             ///< 数据源配置
  bool active_high{DEFAULT_ACTIVE_HIGH}; ///< 有效电平（true=高电平有效，false=低电平有效）
  int ros2_joy_index{-1};       ///< ROS2 Joy消息中的按钮索引（-1表示未映射）
  Debouncer debouncer{DEFAULT_DEBOUNCE_MS}; ///< 去抖器实例（默认10ms去抖）
};

/**
 * @brief 离散轴配置
 *
 * 定义离散轴（如上下/左右开关）的配置参数
 * 基于两个位源（上/下）计算轴值
 */
struct DiscreteAxis
{
  int code{ABS_MISC};     ///< 轴代码
  RegBitSource up;        ///< 上方向位源
  RegBitSource down;      ///< 下方向位源
  double lpf_alpha{1.0};  ///< 低通滤波系数
  LowPassFilter lpf{1.0}; ///< 低通滤波器实例
};

/**
 * @brief 主配置结构
 *
 * 包含Modbus摇杆的所有配置参数
 */
struct Config
{
  // 串口通信参数
  std::string serial_port{DEFAULT_SERIAL_PORT}; ///< 串口设备路径
  int baud{DEFAULT_BAUD_RATE};                 ///< 波特率
  char parity{DEFAULT_PARITY};                 ///< 校验位（N-无校验，E-偶校验，O-奇校验）
  int data_bits{DEFAULT_DATA_BITS};            ///< 数据位
  int stop_bits{DEFAULT_STOP_BITS};            ///< 停止位
  int slave_id{DEFAULT_SLAVE_ID};              ///< Modbus从站ID（单个值，向后兼容）
  std::vector<int> slave_ids{DEFAULT_SLAVE_ID}; ///< Modbus从站ID列表（支持多从站轮询）

  // 阀控板配置
  bool enable_valve_control{false};                  ///< 是否启用阀控板功能
  std::string valve_control_config_file{""};        ///< 阀控板配置文件路径（如果为空则使用当前配置文件的valve_control段）
  
  // 阀控板串口通信参数（独立串口）
  std::string valve_control_port{DEFAULT_SERIAL_PORT}; ///< 阀控板串口设备路径
  int valve_control_baud{DEFAULT_BAUD_RATE};         ///< 阀控板波特率
  char valve_control_parity{DEFAULT_PARITY};         ///< 阀控板校验位
  int valve_control_data_bits{DEFAULT_DATA_BITS};   ///< 阀控板数据位
  int valve_control_stop_bits{DEFAULT_STOP_BITS};   ///< 阀控板停止位
  int valve_control_slave_id{DEFAULT_SLAVE_ID};      ///< 阀控板Modbus从站ID

  // 系统参数
  double poll_hz{DEFAULT_POLL_HZ}; ///< 轮询频率（Hz）
  int bus_error_threshold{2};      ///< 总线错误判定时间阈值（秒），范围2-60，默认2秒
  bool enable_modbus{false};       ///< 是否启用Modbus功能（默认false，即USB模式）

  // 电压映射参数（必须通过配置文件设置）
  double min_voltage{0.0};  ///< 最小电压值 (V) - 必须通过配置文件设置
  double mid_voltage{0.0};  ///< 中间电压值 (V) - 必须通过配置文件设置
  double max_voltage{0.0};  ///< 最大电压值 (V) - 必须通过配置文件设置
  uint16_t modbus_max_value{4095};  ///< Modbus寄存器最大值（默认4095，对应0-10V范围）

  // 设备配置
  std::vector<AxisMap> axes;               ///< 连续轴配置数组
  std::vector<ButtonMap> buttons;          ///< 按钮配置数组
  std::vector<DiscreteAxis> discrete_axes; ///< 离散轴配置数组

  // 调试配置（可选，如果未设置则使用默认值）
  DebugConfig debug_config;               ///< 调试配置（运行时控制调试开关）

  // 只读访问器（封装内部结构）
  const std::string &getSerialPort() const { return serial_port; }
  int getBaud() const { return baud; }
  char getParity() const { return parity; }
  int getDataBits() const { return data_bits; }
  int getStopBits() const { return stop_bits; }
  int getSlaveId() const { return slave_id; }
  const std::vector<int>& getSlaveIds() const { return slave_ids; }
  double getPollHz() const { return poll_hz; }
  int getBusErrorThreshold() const { return bus_error_threshold; }
  const std::vector<AxisMap> &getAxes() const { return axes; }
  std::vector<AxisMap> &getAxes() { return axes; }
  const std::vector<ButtonMap> &getButtons() const { return buttons; }
  std::vector<ButtonMap> &getButtons() { return buttons; }
  const std::vector<DiscreteAxis> &getDiscreteAxes() const { return discrete_axes; }
  std::vector<DiscreteAxis> &getDiscreteAxes() { return discrete_axes; }
  
  // 电压映射参数访问器
  double getMinVoltage() const { return min_voltage; }
  double getMidVoltage() const { return mid_voltage; }
  double getMaxVoltage() const { return max_voltage; }
  uint16_t getModbusMaxValue() const { return modbus_max_value; }
  
  // Modbus功能访问器
  bool getEnableModbus() const { return enable_modbus; }
  
  // 阀控板配置访问器
  bool getEnableValveControl() const { return enable_valve_control; }
  const std::string& getValveControlConfigFile() const { return valve_control_config_file; }
  const std::string& getValveControlPort() const { return valve_control_port; }
  int getValveControlBaud() const { return valve_control_baud; }
  char getValveControlParity() const { return valve_control_parity; }
  int getValveControlDataBits() const { return valve_control_data_bits; }
  int getValveControlStopBits() const { return valve_control_stop_bits; }
  int getValveControlSlaveId() const { return valve_control_slave_id; }
  
  // 调试配置访问器
  const DebugConfig& getDebugConfig() const { return debug_config; }
  DebugConfig& getDebugConfig() { return debug_config; }
};

// 配置加载器类声明
class ConfigLoader
{
public:
  /**
   * @brief 从文件加载配置
   * @param path 配置文件路径
   * @return 解析后的配置对象
   * @throws std::runtime_error 当文件无法打开或解析失败时
   */
  static Config Load(const std::string &path);
};

// 名称 ↔ 码值转换函数声明

/**
 * @brief 将轴名称字符串转换为对应的轴代码
 * @param name 轴名称（如"ABS_X", "ABS_Y"等）
 * @return 对应的轴代码
 * @throws std::runtime_error 当轴名称未知时
 */
int StrToAbsCode(const std::string &name, const std::string &contextPath = "");

/**
 * @brief 将按键名称字符串转换为对应的按键代码
 * @param name 按键名称（如"BTN_1", "BTN_TRIGGER_HAPPY1"等）
 * @return 对应的按键代码
 * @throws std::runtime_error 当按键名称未知或索引超出范围时
 */
int StrToBtnCode(const std::string &name, const std::string &contextPath = "");

/**
 * @brief 将轴代码转换为对应的轴名称字符串
 * @param code 轴代码
 * @return 对应的轴名称，如果未找到则返回"ABS_UNKNOWN"
 */
std::string AbsCodeToString(int code);

/**
 * @brief 将按键代码转换为对应的按键名称字符串
 * @param code 按键代码
 * @return 对应的按键名称，如果未找到则返回"BTN_UNKNOWN"
 */
std::string BtnCodeToString(int code);
