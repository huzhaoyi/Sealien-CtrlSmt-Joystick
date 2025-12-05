/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:29:00
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:29:00
 * @FilePath: include/modbus_client.h
 * @Description: Modbus RTU客户端头文件
 * 定义Modbus串口通信客户端类，支持读取输入寄存器、保持寄存器和线圈
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <atomic>
#include <chrono>
#include <functional>
#include <modbus/modbus.h>
#include "mapping.h"

/**
 * @brief Modbus RTU客户端类
 * 
 * 该类封装了libmodbus库的功能，提供简化的Modbus RTU通信接口。
 * 支持串口通信，可读取输入寄存器、保持寄存器和线圈数据。
 * 
 * 主要特性：
 * - 自动管理Modbus上下文和连接
 * - 支持批量读取操作以提高效率
 * - 提供位级读取功能
 * - 自动错误处理和日志记录
 */
class ModbusClient {
public:
    /**
     * @brief 构造函数
     * @param port 串口设备路径（如/dev/ttyUSB0）
     * @param baud 波特率
     * @param parity 校验位（N-无校验，E-偶校验，O-奇校验）
     * @param data_bits 数据位
     * @param stop_bits 停止位
     * @param slave_id 从站ID
     * @param bus_error_threshold 总线错误判定时间阈值（秒），范围2-60，默认2秒
     * @param device_type 设备类型标识（用于日志区分，如"采集卡"或"阀控板"），默认为空
     */
    ModbusClient(const std::string& port, int baud, char parity, int data_bits, int stop_bits, int slave_id, int bus_error_threshold = DEFAULT_TIMEOUT_MS / MILLISECONDS_PER_SECOND, const std::string& device_type = "");
    
    /**
     * @brief 析构函数
     * 自动关闭连接并释放资源
     */
    ~ModbusClient();

    /**
     * @brief 建立Modbus连接
     * @return 连接成功返回true，失败返回false
     */
    bool connect();
    
    /**
     * @brief 关闭Modbus连接
     */
    void close();
    
    /**
     * @brief 检查连接状态
     * @return 如果连接正常且上下文有效则返回true
     */
    bool ok() const { return ctx_ != nullptr && connected_; }

    // 批量读取函数 - 提高通信效率
    
    /**
     * @brief 批量读取输入寄存器
     * @param addr 起始地址
     * @param count 读取数量
     * @param out 输出向量，会被调整为count大小
     * @return 读取成功返回true，失败返回false
     */
    bool readInputRange(int addr, int count, std::vector<uint16_t>& out);
    
    /**
     * @brief 批量读取保持寄存器
     * @param addr 起始地址
     * @param count 读取数量
     * @param out 输出向量，会被调整为count大小
     * @return 读取成功返回true，失败返回false
     */
    bool readHoldingRange(int addr, int count, std::vector<uint16_t>& out);
    
    /**
     * @brief 批量读取线圈
     * @param addr 起始地址
     * @param count 读取数量
     * @param out 输出向量，会被调整为count大小
     * @return 读取成功返回true，失败返回false
     */
    bool readCoilRange(int addr, int count, std::vector<uint8_t>& out);

    /**
     * @brief 批量读取离散量输入
     * @param addr 起始地址
     * @param count 读取数量
     * @param out 输出向量，会被调整为count大小
     * @return 读取成功返回true，失败返回false
     */
    bool readDiscreteInputRange(int addr, int count, std::vector<uint8_t>& out);

    /**
     * @brief 写单个线圈
     * @param addr 线圈地址
     * @param value 写入值（true=闭合，false=断开）
     * @return 写入成功返回true，失败返回false
     */
    bool writeSingleCoil(int addr, bool value);

    /**
     * @brief 批量写线圈
     * @param addr 起始地址
     * @param values 写入值向量
     * @return 写入成功返回true，失败返回false
     */
    bool writeMultipleCoils(int addr, const std::vector<bool>& values);

    /**
     * @brief 写单个保持寄存器
     * @param addr 寄存器地址
     * @param value 写入值（0-4095）
     * @return 写入成功返回true，失败返回false
     */
    bool writeSingleRegister(int addr, uint16_t value);

    /**
     * @brief 批量写保持寄存器
     * @param addr 起始地址
     * @param values 写入值向量
     * @return 写入成功返回true，失败返回false
     */
    bool writeMultipleRegisters(int addr, const std::vector<uint16_t>& values);

    // 单个读取函数 - 适用于读取单个值
    
    /**
     * @brief 读取单个输入寄存器
     * @param addr 寄存器地址
     * @param out 输出值
     * @return 读取成功返回true，失败返回false
     */
    bool readInput(int addr, uint16_t& out);
    
    /**
     * @brief 读取单个保持寄存器
     * @param addr 寄存器地址
     * @param out 输出值
     * @return 读取成功返回true，失败返回false
     */
    bool readHolding(int addr, uint16_t& out);
    
    /**
     * @brief 读取单个线圈
     * @param addr 线圈地址
     * @param out 输出值
     * @return 读取成功返回true，失败返回false
     */
    bool readCoil(int addr, uint8_t& out);

    /**
     * @brief 读取单个离散量输入
     * @param addr 离散量输入地址
     * @param out 输出值
     * @return 读取成功返回true，失败返回false
     */
    bool readDiscreteInput(int addr, uint8_t& out);

    /**
     * @brief 根据位源类型读取位值
     * @param s 位源描述（包含类型、地址和位索引）
     * @return 位值（0或1），读取失败时返回0
     */
    int readBit(const RegBitSource& s);

    /**
     * @brief 检查总线错误状态
     * @return 如果检测到总线错误返回true，否则返回false
     */
    bool isBusError() const;

    /**
     * @brief 重置总线错误状态
     */
    void resetBusError();

    /**
     * @brief 获取总线错误信息
     * @return 总线错误信息字符串
     */
    std::string getBusErrorInfo() const;

    /**
     * @brief 更新总线错误状态
     * @param success 当前操作是否成功
     */
    void updateBusErrorStatus(bool success);
    
    /**
     * @brief 设置是否自动更新总线错误状态
     * @param enable 是否启用自动更新（默认true）
     */
    void setAutoUpdateBusError(bool enable) { auto_update_bus_error_ = enable; }
    
    /**
     * @brief 获取是否自动更新总线错误状态
     * @return 是否启用自动更新
     */
    bool isAutoUpdateBusError() const { return auto_update_bus_error_; }

    // 自动重连相关功能
    
    /**
     * @brief 启用自动重连功能
     * @param enable 是否启用自动重连
     * @param reconnect_interval_ms 重连间隔（毫秒）
     * @param max_reconnect_attempts 最大重连尝试次数
     */
    void enableAutoReconnect(bool enable, int reconnect_interval_ms = MODBUS_AUTO_RECONNECT_INTERVAL_MS, int max_reconnect_attempts = MODBUS_AUTO_RECONNECT_MAX_ATTEMPTS);
    
    /**
     * @brief 检查是否启用自动重连
     * @return 启用自动重连返回true，否则返回false
     */
    bool isAutoReconnectEnabled() const { return auto_reconnect_enabled_.load(); }
    
    /**
     * @brief 手动触发重连
     * @return 重连成功返回true，失败返回false
     */
    bool reconnect(bool is_bus_error = false);
    
    /**
     * @brief 获取连接状态详细信息
     * @return 连接状态信息字符串
     */
    std::string getConnectionStatus() const;
    
    /**
     * @brief 设置连接状态变化回调函数
     * @param callback 回调函数，参数为(device_path, connected)
     */
    void setConnectionCallback(std::function<void(const std::string&, bool)> callback);
    
    /**
     * @brief 检查设备是否仍然存在
     * @return 设备存在返回true，否则返回false
     */
    bool checkDeviceExists() const;
    
    /**
     * @brief 获取重连统计信息
     * @return 重连统计信息字符串
     */
    std::string getReconnectStats() const;

    /**
     * @brief 设置Modbus寄存器最大值（用于电压转换）
     * @param max_value Modbus寄存器最大值（默认4095）
     */
    void setModbusMaxValue(uint16_t max_value) { modbus_max_value_ = max_value; }

    /**
     * @brief 获取Modbus寄存器最大值
     * @return Modbus寄存器最大值
     */
    uint16_t getModbusMaxValue() const { return modbus_max_value_; }

    /**
     * @brief 设置Modbus从站ID（动态切换）
     * @param slave_id 从站ID
     * @return 设置成功返回true，失败返回false
     */
    bool setSlaveId(int slave_id);
    
    /**
     * @brief 获取当前Modbus从站ID
     * @return 当前从站ID
     */
    int getSlaveId() const { return slave_id_; }

private:
    modbus_t* ctx_{nullptr};    ///< Modbus上下文指针
    bool connected_{false};      ///< 连接状态标志
    int bus_error_threshold_;    ///< 总线错误判定时间阈值（秒）
    int consecutive_errors_;     ///< 连续错误计数
    std::chrono::steady_clock::time_point first_error_time_; ///< 第一次错误时间
    bool bus_error_detected_;    ///< 总线错误检测标志
    
    // 自动重连相关成员
    std::atomic<bool> auto_reconnect_enabled_{false}; ///< 自动重连启用标志
    std::atomic<int> reconnect_interval_ms_{5000};     ///< 重连间隔（毫秒）
    std::atomic<int> max_reconnect_attempts_{10};      ///< 最大重连尝试次数
    std::atomic<int> reconnect_attempts_{0};           ///< 当前重连尝试次数
    std::chrono::steady_clock::time_point last_reconnect_attempt_; ///< 最后重连尝试时间
    std::chrono::steady_clock::time_point last_successful_operation_; ///< 最后成功操作时间
    std::function<void(const std::string&, bool)> connection_callback_; ///< 连接状态变化回调
    
    // 串口配置参数（用于重连时重新创建上下文）
    std::string port_;
    std::string device_type_;  ///< 设备类型标识（用于日志区分）
    int baud_;
    char parity_;
    int data_bits_;
    int stop_bits_;
    int slave_id_;
    
    // Modbus寄存器最大值（用于电压转换）
    uint16_t modbus_max_value_{4095};  ///< Modbus寄存器最大值（默认4095，对应标准12位ADC）
    
    // 总线错误状态自动更新控制
    bool auto_update_bus_error_{true};  ///< 是否自动更新总线错误状态（默认true）
    
    /**
     * @brief 重新创建Modbus上下文
     * @return 创建成功返回true，失败返回false
     */
    bool recreateContext();
    
    /**
     * @brief 检查是否可以尝试重连
     * @return 可以重连返回true，否则返回false
     */
    bool canAttemptReconnect() const;
    
    /**
     * @brief 更新重连统计信息
     * @param success 重连是否成功
     */
    void updateReconnectStats(bool success);
};
