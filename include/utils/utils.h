/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:29:08
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:29:08
 * @FilePath: include/utils.h
 * @Description: 工具函数和日志宏头文件
 * 提供时间格式化、彩色日志输出等通用功能
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */
#pragma once
#include <chrono>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include "../core/constants.h"
#include "thread_safe_logger.h"
#include "debug_config.h"

// 读映射小工具
#include <map>
#include <cstdint>

/**
 * @brief 从寄存器映射中获取指定地址的值
 * @param regmap 寄存器映射
 * @param addr 地址
 * @param def 默认值
 * @return 查询到的值或默认值
 */
inline int GetRegValue(const std::map<int,uint16_t>& regmap, int addr, int def=0){
    auto it = regmap.find(addr);
    return it==regmap.end()? def : static_cast<int>(it->second);
}

/**
 * @brief 从线圈映射中获取指定地址的值
 * @param cmap 线圈映射
 * @param addr 地址
 * @param def 默认值
 * @return 查询到的值或默认值
 */
inline int GetCoilValue(const std::map<int,uint8_t>& cmap, int addr, int def=0){
    auto it = cmap.find(addr);
    return it==cmap.end()? def : static_cast<int>(it->second);
}

// ANSI 颜色代码定义
#define ANSI_RESET   "\033[0m"    ///< 重置所有格式
#define ANSI_RED     "\033[31m"   ///< 红色
#define ANSI_GREEN   "\033[32m"   ///< 绿色
#define ANSI_YELLOW  "\033[33m"   ///< 黄色
#define ANSI_BLUE    "\033[34m"   ///< 蓝色
#define ANSI_MAGENTA "\033[35m"   ///< 洋红色
#define ANSI_CYAN    "\033[36m"   ///< 青色
#define ANSI_WHITE   "\033[37m"   ///< 白色
#define ANSI_BOLD    "\033[1m"    ///< 粗体

// 高亮颜色
#define ANSI_BRIGHT_RED     "\033[91m"   ///< 亮红色
#define ANSI_BRIGHT_GREEN   "\033[92m"   ///< 亮绿色
#define ANSI_BRIGHT_YELLOW  "\033[93m"   ///< 亮黄色
#define ANSI_BRIGHT_BLUE    "\033[94m"   ///< 亮蓝色
#define ANSI_BRIGHT_MAGENTA "\033[95m"   ///< 亮洋红色
#define ANSI_BRIGHT_CYAN    "\033[96m"   ///< 亮青色
#define ANSI_BRIGHT_WHITE   "\033[97m"   ///< 亮白色

/**
 * @brief 获取当前时间的格式化字符串
 * @return 格式为"YYYY-MM-DD HH:MM:SS.mmm"的时间字符串
 * 
 * 该函数获取系统当前时间，包括毫秒精度，并格式化为易读的字符串。
 * 使用本地时间，适合日志记录和调试输出。
 */
inline std::string NowTimeString() {
    using namespace std::chrono;
    auto tp = system_clock::now();                    // 获取当前系统时间点
    auto t = system_clock::to_time_t(tp);            // 转换为time_t
    auto ms = duration_cast<milliseconds>(tp.time_since_epoch()) % 1000;  // 提取毫秒部分
    
    std::tm tm{};
    localtime_r(&t, &tm);                           // 转换为本地时间结构
    
    // 格式化输出：日期时间 + 毫秒
    std::ostringstream oss;
    oss << std::put_time(&tm, "%F %T") << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}

// 彩色日志宏定义：输出格式为 [YYYY-MM-DD HH:MM:SS.mmm] [LEVEL] 消息
// 使用do-while(0)结构确保宏在if-else语句中正确展开

/**
 * @brief 信息日志宏（线程安全版本）
 * @param msg 日志消息（支持流式输出）
 * 
 * 输出绿色信息日志，格式：[I 时间戳] 消息内容
 * 用于记录程序正常运行状态和重要信息
 * 使用线程安全日志管理器避免多线程冲突
 */
#define LOGI(msg) do { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logInfo(oss.str()); \
} while(0)

/**
 * @brief 警告日志宏（线程安全版本）
 * @param msg 日志消息（支持流式输出）
 * 
 * 输出黄色警告日志，格式：[W 时间戳] 消息内容
 * 用于记录需要注意但不影响程序运行的情况
 * 使用线程安全日志管理器避免多线程冲突
 */
#define LOGW(msg) do { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logWarning(oss.str()); \
} while(0)

/**
 * @brief 错误日志宏（线程安全版本）
 * @param msg 日志消息（支持流式输出）
 * 
 * 输出红色粗体错误日志，格式：[E 时间戳] 消息内容
 * 用于记录程序错误和异常情况
 * 使用线程安全日志管理器避免多线程冲突
 */
#define LOGE(msg) do { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logError(oss.str()); \
} while(0)

// 可选调试日志（仅在定义 ENABLE_DEBUG_LOGS 时启用，线程安全版本）
#if ENABLE_DEBUG_LOGS
#define LOGD(msg) do { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logDebug(oss.str()); \
} while(0)
#else
#define LOGD(msg) do { } while(0)
#endif

// ============================================================================
// 功能模块专用调试打印宏（独立控制）
// ============================================================================

/**
 * @brief Core模块调试打印宏（线程安全版本）
 * @param msg 调试消息（支持流式输出）
 * 
 * 输出蓝色Core模块调试信息，格式：[CORE] 消息内容
 * 用于Core模块的调试输出，通过运行时配置控制开关
 * 使用线程安全日志管理器避免多线程冲突
 */
#define DEBUG_CORE_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugCore()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_INFO, "CORE", oss.str()); \
    } \
} while(0)

// ============================================================================
// 采集卡分级调试打印宏（精细控制）
// ============================================================================

/**
 * @brief 采集卡通信调试打印宏（连接、错误等）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_MODBUS_COMM_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugModbusComm()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "ACQUISITION-COMM", oss.str()); \
    } \
} while(0)

/**
 * @brief 采集卡输入寄存器调试打印宏（模拟量输入）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_MODBUS_INPUT_REG_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugModbusInputReg()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "ACQUISITION-INPUT", oss.str()); \
    } \
} while(0)

/**
 * @brief 采集卡保持寄存器调试打印宏（模拟量输出）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_MODBUS_HOLDING_REG_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugModbusHoldingReg()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "ACQUISITION-HOLDING", oss.str()); \
    } \
} while(0)

/**
 * @brief 采集卡线圈调试打印宏（数字输出）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_MODBUS_COIL_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugModbusCoil()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "ACQUISITION-COIL", oss.str()); \
    } \
} while(0)

/**
 * @brief 采集卡离散输入调试打印宏（数字输入）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_MODBUS_DISCRETE_INPUT_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugModbusDiscreteInput()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "ACQUISITION-DISCRETE", oss.str()); \
    } \
} while(0)

/**
 * @brief 采集卡电压转换调试打印宏（电压/电流值显示）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_MODBUS_VOLTAGE_CONV_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugModbusVoltageConv()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "ACQUISITION-VOLTAGE", oss.str()); \
    } \
} while(0)

/**
 * @brief 采集卡写入操作调试打印宏
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_MODBUS_WRITE_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugModbusWrite()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "ACQUISITION-WRITE", oss.str()); \
    } \
} while(0)

/**
 * @brief 采集卡十六进制数据帧调试打印宏
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_MODBUS_HEX_FRAME_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugModbusHexFrame()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "ACQUISITION-HEX", oss.str()); \
    } \
} while(0)

/**
 * @brief Joystick模块调试打印宏
 * @param msg 调试消息（支持流式输出）
 * 
 * 输出青色Joystick模块调试信息，格式：[JOYSTICK] 消息内容
 * 用于Joystick模块的调试输出，通过运行时配置控制开关
 */
#define DEBUG_JOYSTICK_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugJoystick()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_JOYSTICK, "JOYSTICK", oss.str()); \
    } \
} while(0)

// ============================================================================
// 阀控板分级调试打印宏（精细控制）
// ============================================================================

/**
 * @brief 阀控板基础调试打印宏（连接、错误等）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_VALVE_CONTROL_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControl()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-CTRL", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板状态读取调试打印宏（总开关）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_VALVE_CONTROL_STATUS_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlStatus()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-STATUS", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板所有寄存器原始数据调试打印宏
 */
#define DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlStatusRegisters()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-STATUS", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板关键寄存器调试打印宏
 */
#define DEBUG_VALVE_CONTROL_STATUS_KEY_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlStatusKey()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-STATUS", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板阀电流值调试打印宏
 */
#define DEBUG_VALVE_CONTROL_STATUS_CURRENT_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlStatusCurrent()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-STATUS", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板传感器数据调试打印宏
 */
#define DEBUG_VALVE_CONTROL_STATUS_SENSOR_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlStatusSensor()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-STATUS", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板编码器数据调试打印宏
 */
#define DEBUG_VALVE_CONTROL_STATUS_ENCODER_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlStatusEncoder()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-STATUS", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板接近开关调试打印宏
 */
#define DEBUG_VALVE_CONTROL_STATUS_PROXIMITY_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlStatusProximity()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-STATUS", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板环境传感器（湿度/温度/板载）调试打印宏
 */
#define DEBUG_VALVE_CONTROL_STATUS_ENV_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlStatusEnv()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-STATUS", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板传感器状态调试打印宏
 */
#define DEBUG_VALVE_CONTROL_STATUS_SENSOR_STATUS_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlStatusSensorStatus()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-STATUS", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板阀状态调试打印宏
 */
#define DEBUG_VALVE_CONTROL_STATUS_VALVE_STATUS_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlStatusValveStatus()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-STATUS", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板写入操作调试打印宏
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_VALVE_CONTROL_WRITE_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlWrite()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "VALVE-WRITE", oss.str()); \
    } \
} while(0)

/**
 * @brief 阀控板摇杆消息调试打印宏
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_VALVE_CONTROL_JOYSTICK_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugValveControlJoystick()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_JOYSTICK, "VALVE-JOY", oss.str()); \
    } \
} while(0)

// ============================================================================
// ROS2分级调试打印宏（精细控制）
// ============================================================================

/**
 * @brief ROS2基础通信调试打印宏（连接、错误等）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_ROS2_COMM_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugROS2Comm()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_ROS2, "ROS2-COMM", oss.str()); \
    } \
} while(0)

/**
 * @brief ROS2轴数据调试打印宏（Axes数据打印）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_ROS2_AXES_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugROS2Axes()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_ROS2, "ROS2-AXES", oss.str()); \
    } \
} while(0)

/**
 * @brief ROS2按钮数据调试打印宏（Buttons数据打印）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_ROS2_BUTTONS_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugROS2Buttons()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_ROS2, "ROS2-BUTTONS", oss.str()); \
    } \
} while(0)

/**
 * @brief ROS2 Twist消息调试打印宏（Twist数据打印）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_ROS2_TWIST_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugROS2Twist()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_ROS2, "ROS2-TWIST", oss.str()); \
    } \
} while(0)

/**
 * @brief ROS2状态消息调试打印宏（Status数据打印）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_ROS2_STATUS_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugROS2Status()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_ROS2, "ROS2-STATUS", oss.str()); \
    } \
} while(0)

/**
 * @brief ROS2消息发布调试打印宏（发布操作日志）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_ROS2_PUBLISH_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugROS2Publish()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_ROS2, "ROS2-PUBLISH", oss.str()); \
    } \
} while(0)

/**
 * @brief ROS2订阅调试打印宏（订阅操作日志）
 * @param msg 调试消息（支持流式输出）
 */
#define DEBUG_ROS2_SUBSCRIBE_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugROS2Subscribe()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_ROS2, "ROS2-SUBSCRIBE", oss.str()); \
    } \
} while(0)

/**
 * @brief ROS2模块调试打印宏（兼容性宏，保持向后兼容）
 * @param msg 调试消息（支持流式输出）
 * 
 * 输出绿色ROS2模块调试信息，格式：[ROS2] 消息内容
 * 用于ROS2模块的调试输出，通过运行时配置控制开关
 */
#define DEBUG_ROS2_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugROS2Comm()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_ROS2, "ROS2", oss.str()); \
    } \
} while(0)

/**
 * @brief Utils模块调试打印宏
 * @param msg 调试消息（支持流式输出）
 * 
 * 输出白色Utils模块调试信息，格式：[UTILS] 消息内容
 * 用于Utils模块的调试输出，通过运行时配置文件（config/debug.yaml）控制开关
 */
#define DEBUG_UTILS_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugUtils()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_DEBUG, "UTILS", oss.str()); \
    } \
} while(0)

/**
 * @brief Mapping模块调试打印宏
 * @param msg 调试消息（支持流式输出）
 * 
 * 输出黄色Mapping模块调试信息，格式：[MAPPING] 消息内容
 * 用于Mapping模块的调试输出，通过运行时配置文件（config/debug.yaml）控制开关
 */
#define DEBUG_MAPPING_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugMapping()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_WARNING, "MAPPING", oss.str()); \
    } \
} while(0)

/**
 * @brief UInput模块调试打印宏
 * @param msg 调试消息（支持流式输出）
 * 
 * 输出红色UInput模块调试信息，格式：[UINPUT] 消息内容
 * 用于UInput模块的调试输出，通过运行时配置文件（config/debug.yaml）控制开关
 */
#define DEBUG_UINPUT_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugUInput()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_UINPUT, "UINPUT", oss.str()); \
    } \
} while(0)

/**
 * @brief Config模块调试打印宏
 * @param msg 调试消息（支持流式输出）
 * 
 * 输出蓝色Config模块调试信息，格式：[CONFIG] 消息内容
 * 用于Config模块的调试输出，通过运行时配置文件（config/debug.yaml）控制开关
 */
#define DEBUG_CONFIG_LOG(msg) do { \
    if (DebugConfigManager::getInstance().isDebugConfig()) { \
        std::ostringstream oss; \
        oss << msg; \
        ThreadSafeLogger::getInstance().logModuleDebug(COLOR_CONFIG, "CONFIG", oss.str()); \
    } \
} while(0)

// ============================================================================
// 错误和警告日志宏（模块名保持模块颜色，内容使用错误/警告颜色）
// ============================================================================

// Core模块错误和警告（线程安全版本，受debug配置控制）
#define DEBUG_CORE_ERROR(msg) do { \
    if (DebugConfigManager::getInstance().isDebugCore()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleError(COLOR_INFO, "CORE", oss.str()); \
    } \
} while(0)
#define DEBUG_CORE_WARNING(msg) do { \
    if (DebugConfigManager::getInstance().isDebugCore()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleWarning(COLOR_INFO, "CORE", oss.str()); \
    } \
} while(0)

// 采集卡模块错误和警告（线程安全版本，受debug配置控制）
#define DEBUG_MODBUS_ERROR(msg) do { \
    if (DebugConfigManager::getInstance().isDebugModbusComm()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleError(COLOR_MODBUS, "ACQUISITION", oss.str()); \
    } \
} while(0)
#define DEBUG_MODBUS_WARNING(msg) do { \
    if (DebugConfigManager::getInstance().isDebugModbusComm()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleWarning(COLOR_MODBUS, "ACQUISITION", oss.str()); \
    } \
} while(0)

// Joystick模块错误和警告（线程安全版本，受debug配置控制）
#define DEBUG_JOYSTICK_ERROR(msg) do { \
    if (DebugConfigManager::getInstance().isDebugJoystick()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleError(COLOR_JOYSTICK, "JOYSTICK", oss.str()); \
    } \
} while(0)
#define DEBUG_JOYSTICK_WARNING(msg) do { \
    if (DebugConfigManager::getInstance().isDebugJoystick()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleWarning(COLOR_JOYSTICK, "JOYSTICK", oss.str()); \
    } \
} while(0)

// ROS2模块错误和警告（线程安全版本，受debug配置控制）
#define DEBUG_ROS2_ERROR(msg) do { \
    if (DebugConfigManager::getInstance().isDebugROS2Comm()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleError(COLOR_ROS2, "ROS2", oss.str()); \
    } \
} while(0)
#define DEBUG_ROS2_WARNING(msg) do { \
    if (DebugConfigManager::getInstance().isDebugROS2Comm()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleWarning(COLOR_ROS2, "ROS2", oss.str()); \
    } \
} while(0)

// UInput模块错误和警告（线程安全版本，受debug配置控制）
#define DEBUG_UINPUT_ERROR(msg) do { \
    if (DebugConfigManager::getInstance().isDebugUInput()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleError(COLOR_UINPUT, "UINPUT", oss.str()); \
    } \
} while(0)
#define DEBUG_UINPUT_WARNING(msg) do { \
    if (DebugConfigManager::getInstance().isDebugUInput()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleWarning(COLOR_UINPUT, "UINPUT", oss.str()); \
    } \
} while(0)

// Config模块错误和警告（线程安全版本，受debug配置控制）
#define DEBUG_CONFIG_ERROR(msg) do { \
    if (DebugConfigManager::getInstance().isDebugConfig()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleError(COLOR_CONFIG, "CONFIG", oss.str()); \
    } \
} while(0)
#define DEBUG_CONFIG_WARNING(msg) do { \
    if (DebugConfigManager::getInstance().isDebugConfig()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleWarning(COLOR_CONFIG, "CONFIG", oss.str()); \
    } \
} while(0)

// Mapping模块错误和警告（线程安全版本，受debug配置控制）
#define DEBUG_MAPPING_ERROR(msg) do { \
    if (DebugConfigManager::getInstance().isDebugMapping()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleError(COLOR_WARNING, "MAPPING", oss.str()); \
    } \
} while(0)
#define DEBUG_MAPPING_WARNING(msg) do { \
    if (DebugConfigManager::getInstance().isDebugMapping()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleWarning(COLOR_WARNING, "MAPPING", oss.str()); \
    } \
} while(0)

// Utils模块错误和警告（线程安全版本，受debug配置控制）
#define DEBUG_UTILS_ERROR(msg) do { \
    if (DebugConfigManager::getInstance().isDebugUtils()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleError(COLOR_DEBUG, "UTILS", oss.str()); \
    } \
} while(0)
#define DEBUG_UTILS_WARNING(msg) do { \
    if (DebugConfigManager::getInstance().isDebugUtils()) { \
    std::ostringstream oss; \
    oss << msg; \
    ThreadSafeLogger::getInstance().logModuleWarning(COLOR_DEBUG, "UTILS", oss.str()); \
    } \
} while(0)
