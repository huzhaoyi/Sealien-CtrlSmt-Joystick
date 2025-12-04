/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:29:24
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:29:24
 * @FilePath: include/constants.h
 * @Description: 项目常量定义头文件
 * 定义项目中所有可配置的常量、限制值和默认参数，便于统一管理和修改
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */
#pragma once

// ============================================================================
// 系统配置常量
// ============================================================================

// 默认配置文件路径
#define DEFAULT_CONFIG_PATH "../config/AMSAMOTION_IO8R.yaml"

// 默认串口配置
#define DEFAULT_SERIAL_PORT "/dev/ttyUSB0"
#define DEFAULT_BAUD_RATE 115200
#define DEFAULT_PARITY 'N'
#define DEFAULT_DATA_BITS 8
#define DEFAULT_STOP_BITS 1
#define DEFAULT_SLAVE_ID 1

// 默认轮询频率
#define DEFAULT_POLL_HZ 120.0

// ============================================================================
// 轴和输入设备相关常量
// ============================================================================

// 轴值范围（Linux输入子系统标准）
#define AXIS_MIN_VALUE -32767
#define AXIS_MAX_VALUE 32767
#define AXIS_CENTER_VALUE 0

// 轴值分辨率（16位精度）
#define AXIS_RESOLUTION 16

// 默认轴配置参数
#define DEFAULT_RAW_MIN 0
#define DEFAULT_RAW_MAX 4095
#define DEFAULT_DEADZONE 0.02
#define DEFAULT_LPF_ALPHA 0.3
#define DEFAULT_INVERT false

// 死区限制（避免完全阻塞信号）
#define MAX_DEADZONE 0.49
#define MIN_DEADZONE 0.0

// USB摇杆死区配置
#define USB_JOYSTICK_DEFAULT_DEADZONE 0.12
#define USB_JOYSTICK_MIN_DEADZONE 0.05
#define USB_JOYSTICK_MAX_DEADZONE 0.30
#define USB_JOYSTICK_DEFAULT_SMOOTHING_FACTOR 0.3

// 低通滤波器系数范围
#define MIN_LPF_ALPHA 0.0
#define MAX_LPF_ALPHA 1.0

// ============================================================================
// 按钮和去抖相关常量
// ============================================================================

// 按钮状态值
#define BUTTON_RELEASE 0
#define BUTTON_PRESS 1
#define BUTTON_REPEAT 2

// 默认去抖时间（毫秒）
#define DEFAULT_DEBOUNCE_MS 10
#define MIN_DEBOUNCE_MS 1
#define MAX_DEBOUNCE_MS 1000

// 按钮有效电平默认值
#define DEFAULT_ACTIVE_HIGH true

// ============================================================================
// Modbus通信相关常量
// ============================================================================

// 寄存器地址范围限制
#define MIN_REGISTER_ADDR 0
#define MAX_REGISTER_ADDR 65535

// 线圈地址范围限制
#define MIN_COIL_ADDR 0
#define MAX_COIL_ADDR 65535

// 位索引范围
#define MIN_BIT_INDEX 0
#define MAX_BIT_INDEX 15
#define INVALID_BIT_INDEX -1

// 批量读取的最大数量限制
#define MAX_BATCH_READ_COUNT 125

// 数据类型大小
#define MODBUS_REGISTER_SIZE 16
#define MODBUS_COIL_SIZE 8

// ============================================================================
// 设备标识常量
// ============================================================================

// 虚拟设备标识
#define VIRTUAL_DEVICE_NAME "Modbus Virtual Joystick (cfg)"
#define VIRTUAL_DEVICE_VENDOR_ID 0x23A7
#define VIRTUAL_DEVICE_PRODUCT_ID 0x0002
#define VIRTUAL_DEVICE_VERSION 1

// ============================================================================
// 时间相关常量
// ============================================================================

// 时间单位转换
#define MILLISECONDS_PER_SECOND 1000
#define MICROSECONDS_PER_SECOND 1000000
#define NANOSECONDS_PER_SECOND 1000000000

// 默认超时时间
#define DEFAULT_TIMEOUT_MS 1000
#define CONNECTION_TIMEOUT_MS 5000

// ============================================================================
// 错误处理和日志相关常量
// ============================================================================

// 日志级别
#define LOG_LEVEL_INFO 0
#define LOG_LEVEL_WARN 1
#define LOG_LEVEL_ERROR 2
#define LOG_LEVEL_DEBUG 3

// ============================================================================
// 颜色定义（ANSI转义序列）
// ============================================================================

// 基础颜色
#define COLOR_RESET "\033[0m"
#define COLOR_BOLD "\033[1m"
#define COLOR_DIM "\033[2m"
#define COLOR_ITALIC "\033[3m"
#define COLOR_UNDERLINE "\033[4m"

// 前景色
#define COLOR_BLACK "\033[30m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"

// 背景色
#define COLOR_BG_BLACK "\033[40m"
#define COLOR_BG_RED "\033[41m"
#define COLOR_BG_GREEN "\033[42m"
#define COLOR_BG_YELLOW "\033[43m"
#define COLOR_BG_BLUE "\033[44m"
#define COLOR_BG_MAGENTA "\033[45m"
#define COLOR_BG_CYAN "\033[46m"
#define COLOR_BG_WHITE "\033[47m"

// 高亮颜色
#define COLOR_BRIGHT_BLACK "\033[90m"
#define COLOR_BRIGHT_RED "\033[91m"
#define COLOR_BRIGHT_GREEN "\033[92m"
#define COLOR_BRIGHT_YELLOW "\033[93m"
#define COLOR_BRIGHT_BLUE "\033[94m"
#define COLOR_BRIGHT_MAGENTA "\033[95m"
#define COLOR_BRIGHT_CYAN "\033[96m"
#define COLOR_BRIGHT_WHITE "\033[97m"

// 模块专用颜色
#define COLOR_ERROR COLOR_BRIGHT_RED COLOR_BOLD
#define COLOR_WARNING COLOR_BRIGHT_YELLOW COLOR_BOLD
#define COLOR_SUCCESS COLOR_BRIGHT_GREEN COLOR_BOLD
#define COLOR_INFO COLOR_BRIGHT_CYAN
#define COLOR_DEBUG COLOR_BRIGHT_MAGENTA
#define COLOR_MODBUS COLOR_BRIGHT_BLUE
#define COLOR_JOYSTICK COLOR_BRIGHT_GREEN
#define COLOR_ROS2 COLOR_BRIGHT_MAGENTA
#define COLOR_UINPUT COLOR_BRIGHT_CYAN
#define COLOR_CONFIG COLOR_BRIGHT_YELLOW

// 组合颜色定义（模块名 + 内容颜色）
// 注意：这些颜色常量已废弃，请使用ThreadSafeLogger中的方法
// 保留这些定义以避免编译错误，但实际使用线程安全日志宏

// 最大错误重试次数
#define MAX_RETRY_COUNT 3
#define RETRY_DELAY_MS 100

// ============================================================================
// 内存和缓冲区相关常量
// ============================================================================

// 默认缓冲区大小
#define DEFAULT_BUFFER_SIZE 1024
#define MAX_BUFFER_SIZE 65536

// 字符串长度限制
#define MAX_DEVICE_NAME_LENGTH 256
#define MAX_CONFIG_PATH_LENGTH 512

// ============================================================================
// 编译时配置选项
// ============================================================================

// 调试模式开关
#ifdef NDEBUG
#define ENABLE_DEBUG_LOGS 0
#else
#define ENABLE_DEBUG_LOGS 1
#endif

// ============================================================================
// 功能模块调试开关（已废弃）
// ============================================================================
// 注意：所有调试开关现在通过运行时配置文件（config/debug.yaml）控制
// 这些编译时宏定义已不再使用，保留仅为向后兼容
// 实际调试开关由 DebugConfigManager 在运行时从配置文件加载
// ============================================================================

// 性能优化选项
#define ENABLE_OPTIMIZATIONS 1
#define ENABLE_SIMD_OPTIMIZATIONS 0

// 安全特性开关
#define ENABLE_BOUNDS_CHECKING 1
#define ENABLE_INPUT_VALIDATION 1

// ============================================================================
// 版本信息常量
// ============================================================================

#define PROJECT_VERSION_MAJOR 1
#define PROJECT_VERSION_MINOR 0
#define PROJECT_VERSION_PATCH 0
#define PROJECT_VERSION_STRING "1.0.0"

#define PROJECT_DESCRIPTION "Industrial Modbus to Virtual Joystick Converter"
#define PROJECT_AUTHOR "SRS-HUZY"
#define PROJECT_COPYRIGHT "Copyright (c) 2025 by Sealien Robotics, All Rights Reserved."

// ============================================================================
// 兼容性常量
// ============================================================================

// 支持的Modbus协议版本
#define SUPPORTED_MODBUS_VERSION "RTU"

// 支持的操作系统
#define SUPPORTED_OS_LINUX 1
#define SUPPORTED_OS_WINDOWS 0
#define SUPPORTED_OS_MACOS 0

// 编译器版本要求
#define MIN_GCC_VERSION "7.0"
#define MIN_CLANG_VERSION "5.0"
#define MIN_MSVC_VERSION "2017"

// ============================================================================
// 手柄和ROS2相关常量
// ============================================================================

// 手柄轴数量限制 - 统一配置：4个摇杆 + 8个旋钮 = 12个轴
#define MAX_JOYSTICK_AXES 16
#define MAX_JOYSTICK_BUTTONS 32

// 手柄数据缓存大小 - 统一配置
#define DEFAULT_AXES_CACHE_SIZE 12
#define DEFAULT_BUTTONS_CACHE_SIZE 32

// 轴类型定义
#define MAX_JOYSTICK_STICKS 4      // 4个摇杆
#define MAX_JOYSTICK_KNOBS 8       // 8个旋钮
#define MAX_JOYSTICK_SWITCHES 32   // 32个按键/拨码开关/三段式开关

// 手柄事件类型
#define JS_EVENT_INIT 0x80

// ROS2话题队列大小
#define ROS2_TOPIC_QUEUE_SIZE 10

// ROS2节点名称
#define ROS2_NODE_NAME "sealien_joystick_node"

// ROS2话题名称
#define ROS2_TOPIC_JOY "/sealien_joystick"
#define ROS2_TOPIC_TWIST "/sealien_joystick/cmd_vel"
#define ROS2_TOPIC_STATUS "/sealien_joystick/device_status"
#define ROS2_TOPIC_MODBUS_RAW "/sealien_joystick/modbus_raw"
#define ROS2_TOPIC_CONFIG "/sealien_joystick/config"
#define ROS2_TOPIC_MODBUS_CONTROL "/sealien_joystick/modbus_control"

// ============================================================================
// 时间间隔和频率常量
// ============================================================================

// 手柄检测器扫描间隔
#define JOYSTICK_SCAN_INTERVAL_MS 100
#define JOYSTICK_SCAN_MIN_INTERVAL_MS 100

// USB手柄定时发送配置
#define USB_JOYSTICK_TIMER_INTERVAL_MS 20  // 50Hz定时发送频率
#define USB_JOYSTICK_TIMER_MIN_INTERVAL_MS 10  // 最小发送间隔（100Hz）
#define USB_JOYSTICK_TIMER_MAX_INTERVAL_MS 100  // 最大发送间隔（10Hz）

// 状态显示间隔
#define STATUS_DISPLAY_INTERVAL_COUNT 10  // 每10次循环显示一次状态
#define STATUS_DISPLAY_TIME_MS 1000       // 1秒显示一次状态

// 轮询频率限制
#define MIN_POLL_HZ 1.0
#define MAX_POLL_HZ 1000.0

// ============================================================================
// 数据范围和精度常量
// ============================================================================

// 轴值归一化因子
#define AXIS_NORMALIZATION_FACTOR 32767.0f

// ============================================================================
// 串口和通信常量
// ============================================================================

// 支持的波特率列表
#define SUPPORTED_BAUD_RATES {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200}

// 数据位范围
#define MIN_DATA_BITS 5
#define MAX_DATA_BITS 8

// 停止位范围
#define MIN_STOP_BITS 1
#define MAX_STOP_BITS 2

// 奇偶校验选项
#define PARITY_NONE 'N'
#define PARITY_ODD 'O'
#define PARITY_EVEN 'E'

// ============================================================================
// 心跳和状态监控相关常量
// ============================================================================

// 心跳发布间隔（秒）
#define HEARTBEAT_INTERVAL_SECONDS 3

// 心跳超时检测时间（秒）
#define HEARTBEAT_TIMEOUT_SECONDS 10

// 心跳话题名称
#define HEARTBEAT_TOPIC_NAME "/sealien_joystick/heartbeat"

// 心跳数据字段名称
#define HEARTBEAT_FIELD_TIMESTAMP "timestamp"
#define HEARTBEAT_FIELD_SYSTEM_STATUS "system_status"
#define HEARTBEAT_FIELD_RUN_MODE "run_mode"
#define HEARTBEAT_FIELD_MODBUS_ENABLED "modbus_enabled"
#define HEARTBEAT_FIELD_MODBUS_CONNECTED "modbus_connected"
#define HEARTBEAT_FIELD_ROS2_ENABLED "ros2_enabled"
#define HEARTBEAT_FIELD_ROS2_INITIALIZED "ros2_initialized"
#define HEARTBEAT_FIELD_USB_DEVICES_COUNT "usb_devices_count"
#define HEARTBEAT_FIELD_POLLING_RATE "polling_rate"
#define HEARTBEAT_FIELD_UPTIME "uptime"

// ============================================================================
// USB转串口设备自动恢复相关常量
// ============================================================================

// USB转串口设备扫描间隔（毫秒）
#define USB_SERIAL_SCAN_INTERVAL_MS 1000

// USB转串口设备重连间隔（毫秒）
#define USB_SERIAL_RECONNECT_INTERVAL_MS 5000

// USB转串口设备最大重连尝试次数
#define USB_SERIAL_MAX_RECONNECT_ATTEMPTS 10

// Modbus自动重连默认间隔（毫秒）
#define MODBUS_AUTO_RECONNECT_INTERVAL_MS 5000

// Modbus自动重连默认最大尝试次数（大幅增加，基本相当于无限制）
#define MODBUS_AUTO_RECONNECT_MAX_ATTEMPTS 10000

// Modbus重连最小间隔（毫秒）
#define MODBUS_RECONNECT_MIN_INTERVAL_MS 1000

// 心跳状态值
#define HEARTBEAT_STATUS_RUNNING "running"
#define HEARTBEAT_STATUS_STOPPED "stopped"
#define HEARTBEAT_STATUS_ERROR "error"

// JSON布尔值
#define JSON_TRUE "true"
#define JSON_FALSE "false"