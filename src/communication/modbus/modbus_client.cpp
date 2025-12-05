/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:29:57
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:29:57
 * @FilePath: src/modbus_client.cpp
 * @Description: Modbus RTU客户端实现
 * 提供串口Modbus通信功能，支持读取输入寄存器、保持寄存器和线圈
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "modbus_client.h"
#include "utils.h"
#include "utils/voltage_converter.h"
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>

/**
 * @brief 计算Modbus RTU CRC16校验
 * @param data 数据缓冲区
 * @param length 数据长度
 * @return CRC16校验值
 */
static uint16_t modbus_crc16(const uint8_t* data, int length) {
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief 格式化十六进制数据帧
 * @param data 数据缓冲区
 * @param length 数据长度
 * @return 格式化的十六进制字符串
 */
static std::string format_hex_frame(const uint8_t* data, int length) {
    std::ostringstream oss;
    for (int i = 0; i < length; i++) {
        oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(data[i]);
        if (i < length - 1) {
            oss << " ";
        }
    }
    return oss.str();
}

/**
 * @brief 构造并打印Modbus RTU读取请求的十六进制数据帧
 * @param slave_id 从站ID
 * @param function_code 功能码
 * @param addr 起始地址
 * @param count 数量
 */
static void print_modbus_read_request_hex(int slave_id, uint8_t function_code, int addr, int count) {
    if (!DebugConfigManager::getInstance().isDebugModbusHexFrame()) {
        return;
    }
    
    uint8_t frame[8];
    frame[0] = static_cast<uint8_t>(slave_id);
    frame[1] = function_code;
    frame[2] = static_cast<uint8_t>((addr >> 8) & 0xFF);  // 地址高字节
    frame[3] = static_cast<uint8_t>(addr & 0xFF);          // 地址低字节
    frame[4] = static_cast<uint8_t>((count >> 8) & 0xFF);  // 数量高字节
    frame[5] = static_cast<uint8_t>(count & 0xFF);        // 数量低字节
    
    uint16_t crc = modbus_crc16(frame, 6);
    frame[6] = static_cast<uint8_t>(crc & 0xFF);           // CRC低字节
    frame[7] = static_cast<uint8_t>((crc >> 8) & 0xFF);   // CRC高字节
    
    DEBUG_MODBUS_HEX_FRAME_LOG("-> TX Frame: " << format_hex_frame(frame, 8));
}

/**
 * @brief 构造并打印Modbus RTU写入请求的十六进制数据帧
 * @param slave_id 从站ID
 * @param function_code 功能码
 * @param addr 地址
 * @param value 值（用于单寄存器写入）
 */
static void print_modbus_write_single_hex(int slave_id, uint8_t function_code, int addr, uint16_t value) {
    if (!DebugConfigManager::getInstance().isDebugModbusHexFrame()) {
        return;
    }
    
    uint8_t frame[8];
    frame[0] = static_cast<uint8_t>(slave_id);
    frame[1] = function_code;
    frame[2] = static_cast<uint8_t>((addr >> 8) & 0xFF);
    frame[3] = static_cast<uint8_t>(addr & 0xFF);
    frame[4] = static_cast<uint8_t>((value >> 8) & 0xFF);
    frame[5] = static_cast<uint8_t>(value & 0xFF);
    
    uint16_t crc = modbus_crc16(frame, 6);
    frame[6] = static_cast<uint8_t>(crc & 0xFF);
    frame[7] = static_cast<uint8_t>((crc >> 8) & 0xFF);
    
    DEBUG_MODBUS_HEX_FRAME_LOG("-> TX Frame: " << format_hex_frame(frame, 8));
}

/**
 * @brief 构造并打印Modbus RTU写入单个线圈的十六进制数据帧
 * @param slave_id 从站ID
 * @param addr 地址
 * @param value 值（true=0xFF00, false=0x0000）
 */
static void print_modbus_write_single_coil_hex(int slave_id, int addr, bool value) {
    if (!DebugConfigManager::getInstance().isDebugModbusHexFrame()) {
        return;
    }
    
    uint16_t coil_value = value ? 0xFF00 : 0x0000;
    print_modbus_write_single_hex(slave_id, 0x05, addr, coil_value);  // 功能码0x05: 写单个线圈
}

/**
 * @brief 构造并打印Modbus RTU读取响应的十六进制数据帧
 * @param slave_id 从站ID
 * @param function_code 功能码
 * @param data 读取到的数据
 * @param count 数据数量（寄存器数或字节数）
 * @param is_register 是否为寄存器（true=寄存器，每个2字节；false=位，每个1字节）
 */
static void print_modbus_read_response_hex(int slave_id, uint8_t function_code, const void* data, int count, bool is_register) {
    if (!DebugConfigManager::getInstance().isDebugModbusHexFrame()) {
        return;
    }
    
    int byte_count = is_register ? (count * 2) : ((count + 7) / 8);  // 寄存器：每个2字节；位：每8个位1字节
    int frame_size = 3 + byte_count + 2;  // 从站ID + 功能码 + 字节数 + 数据 + CRC
    std::vector<uint8_t> frame(frame_size);
    
    frame[0] = static_cast<uint8_t>(slave_id);
    frame[1] = function_code;
    frame[2] = static_cast<uint8_t>(byte_count);
    
    if (is_register) {
        // 寄存器数据：每个寄存器2字节（高字节在前）
        const uint16_t* reg_data = static_cast<const uint16_t*>(data);
        for (int i = 0; i < count; i++) {
            frame[3 + i * 2] = static_cast<uint8_t>((reg_data[i] >> 8) & 0xFF);
            frame[3 + i * 2 + 1] = static_cast<uint8_t>(reg_data[i] & 0xFF);
        }
    } else {
        // 位数据：每8个位1字节
        const uint8_t* bit_data = static_cast<const uint8_t*>(data);
        for (int i = 0; i < byte_count; i++) {
            frame[3 + i] = bit_data[i];
        }
    }
    
    uint16_t crc = modbus_crc16(frame.data(), frame_size - 2);
    frame[frame_size - 2] = static_cast<uint8_t>(crc & 0xFF);
    frame[frame_size - 1] = static_cast<uint8_t>((crc >> 8) & 0xFF);
    
    DEBUG_MODBUS_HEX_FRAME_LOG("<- RX Frame: " << format_hex_frame(frame.data(), frame_size));
}

/**
 * @brief 构造并打印Modbus RTU写入响应的十六进制数据帧（回显请求）
 * @param slave_id 从站ID
 * @param function_code 功能码
 * @param addr 地址
 * @param value 值
 */
static void print_modbus_write_response_hex(int slave_id, uint8_t function_code, int addr, uint16_t value) {
    if (!DebugConfigManager::getInstance().isDebugModbusHexFrame()) {
        return;
    }
    
    // 写入响应通常是回显请求
    uint8_t frame[8];
    frame[0] = static_cast<uint8_t>(slave_id);
    frame[1] = function_code;
    frame[2] = static_cast<uint8_t>((addr >> 8) & 0xFF);
    frame[3] = static_cast<uint8_t>(addr & 0xFF);
    frame[4] = static_cast<uint8_t>((value >> 8) & 0xFF);
    frame[5] = static_cast<uint8_t>(value & 0xFF);
    
    uint16_t crc = modbus_crc16(frame, 6);
    frame[6] = static_cast<uint8_t>(crc & 0xFF);
    frame[7] = static_cast<uint8_t>((crc >> 8) & 0xFF);
    
    DEBUG_MODBUS_HEX_FRAME_LOG("<- RX Frame: " << format_hex_frame(frame, 8));
}

/**
 * @brief 构造函数：初始化Modbus RTU客户端
 * @param port 串口设备路径（如/dev/ttyUSB0）
 * @param baud 波特率
 * @param parity 校验位（N-无校验，E-偶校验，O-奇校验）
 * @param data_bits 数据位
 * @param stop_bits 停止位
 * @param slave_id 从站ID
 * @param bus_error_threshold 总线错误判定时间阈值（秒）
 */
ModbusClient::ModbusClient(const std::string& port, int baud, char parity, int data_bits, int stop_bits, int slave_id, int bus_error_threshold, const std::string& device_type)
    : bus_error_threshold_(std::clamp(bus_error_threshold, 2, 60)), 
      consecutive_errors_(0), 
      bus_error_detected_(false),
      port_(port), device_type_(device_type), baud_(baud), parity_(parity), 
      data_bits_(data_bits), stop_bits_(stop_bits), slave_id_(slave_id) {
    // 创建Modbus RTU上下文
    ctx_ = modbus_new_rtu(port.c_str(), baud, parity, data_bits, stop_bits);
    if (!ctx_) {
        DEBUG_MODBUS_ERROR("modbus_new_rtu 失败");
        return;
    }
    
    // 设置从站ID
    modbus_set_slave(ctx_, slave_id);
    
    // 设置超时时间：使用默认超时时间，确保设备有足够时间响应
    struct timeval tv{ .tv_sec = DEFAULT_TIMEOUT_MS / MILLISECONDS_PER_SECOND, .tv_usec = 0 };
    modbus_set_response_timeout(ctx_, tv.tv_sec, tv.tv_usec);
    modbus_set_byte_timeout(ctx_, tv.tv_sec, tv.tv_usec);
    
    // 初始化时间戳
    last_successful_operation_ = std::chrono::steady_clock::now();
    last_reconnect_attempt_ = std::chrono::steady_clock::now();
}

/**
 * @brief 析构函数：清理资源
 */
ModbusClient::~ModbusClient(){
    close();
    if (ctx_) modbus_free(ctx_);
}

/**
 * @brief 建立Modbus连接
 * @return 连接成功返回true，失败返回false
 */
bool ModbusClient::connect(){
    if (!ctx_) {
        DEBUG_MODBUS_ERROR("Modbus 上下文未初始化");
        return false;
    }
    
    if (modbus_connect(ctx_) == -1) {
        DEBUG_MODBUS_ERROR("modbus_connect 失败: " << modbus_strerror(errno));
        return false;
    }
    connected_ = true;
    return true;
}

/**
 * @brief 关闭Modbus连接
 */
void ModbusClient::close(){
    if (connected_) {
        modbus_close(ctx_);
        connected_ = false;
    }
}

/**
 * @brief 批量读取输入寄存器
 * @param addr 起始地址
 * @param count 读取数量
 * @param out 输出向量，会被调整为count大小
 * @return 读取成功返回true，失败返回false
 */
bool ModbusClient::readInputRange(int addr, int count, std::vector<uint16_t>& out){
    out.resize(count);
    DEBUG_MODBUS_INPUT_REG_LOG("从站[" << slave_id_ << "] -> 发送: 读输入寄存器 addr=" << addr << " count=" << count);
    print_modbus_read_request_hex(slave_id_, 0x04, addr, count);  // 功能码0x04: 读输入寄存器
    int rc = modbus_read_input_registers(ctx_, addr, count, out.data());
    bool success = (rc == count);
    
    // 如果读取失败且启用了自动重连，尝试重连
    if (!success && auto_reconnect_enabled_.load()) {
        DEBUG_MODBUS_INPUT_REG_LOG("从站[" << slave_id_ << "] 读取失败，正在尝试重连...");
        if (reconnect()) {
            // 重连成功，重试读取
            rc = modbus_read_input_registers(ctx_, addr, count, out.data());
            success = (rc == count);
            if (success) {
                DEBUG_MODBUS_INPUT_REG_LOG("从站[" << slave_id_ << "] 重连后读取成功");
            }
        }
    }
    
    // 只有在启用自动更新时才更新总线错误状态
    if (auto_update_bus_error_) {
        updateBusErrorStatus(success);
    }
    if (!success) {
        DEBUG_MODBUS_INPUT_REG_LOG("从站[" << slave_id_ << "] <- 响应: 读输入寄存器失败 addr="<<addr<<" count="<<count<<" rc="<<rc<<" errno="<<errno<<" error="<<modbus_strerror(errno));
    } else {
        print_modbus_read_response_hex(slave_id_, 0x04, out.data(), count, true);  // 功能码0x04: 读输入寄存器
        DEBUG_MODBUS_INPUT_REG_LOG("从站[" << slave_id_ << "] <- 响应: 读输入寄存器成功 addr="<<addr<<" count="<<count<<" data=[");
        for (int i = 0; i < count; i++) {
            uint16_t raw_value = out[i];
            DEBUG_MODBUS_INPUT_REG_LOG("  [" << i << "]=" << raw_value);
            
            // 添加电压转换信息（独立显示）
            VoltageConverter::ChannelType channel_type;
            std::string channel_name;
            
            // 根据地址确定通道类型和名称
            int channel_addr = addr + i;
            if (channel_addr >= 0 && channel_addr <= 2) {
                // V1-V3通道：电压输入
                channel_type = VoltageConverter::ChannelType::VOLTAGE_0_10V;
                channel_name = "V" + std::to_string(channel_addr + 1);
            } else if (channel_addr >= 3 && channel_addr <= 5) {
                // A1-A3通道：电流输入
                channel_type = VoltageConverter::ChannelType::CURRENT_0_20MA;
                channel_name = "A" + std::to_string(channel_addr - 2);
            } else {
                // 其他通道：默认电压输入
                channel_type = VoltageConverter::ChannelType::VOLTAGE_0_10V;
                channel_name = "CH" + std::to_string(channel_addr);
            }
            
            // 转换并显示电压/电流值（独立显示，不依赖输入寄存器调试开关）
            auto result = VoltageConverter::convertToVoltage(raw_value, channel_type, modbus_max_value_);
            std::string status = VoltageConverter::getChannelStatus(raw_value, channel_type, modbus_max_value_);
            DEBUG_MODBUS_VOLTAGE_CONV_LOG("    " << channel_name << ": " << VoltageConverter::formatResult(result, channel_type) 
                 << " [" << status << "]");
        }
        DEBUG_MODBUS_INPUT_REG_LOG("]");
    }
    return success;
}

/**
 * @brief 批量读取保持寄存器
 * @param addr 起始地址
 * @param count 读取数量
 * @param out 输出向量，会被调整为count大小
 * @return 读取成功返回true，失败返回false
 */
bool ModbusClient::readHoldingRange(int addr, int count, std::vector<uint16_t>& out){
    out.resize(count);
    DEBUG_MODBUS_HOLDING_REG_LOG("从站[" << slave_id_ << "] -> 发送: 读保持寄存器 addr=" << addr << " count=" << count);
    print_modbus_read_request_hex(slave_id_, 0x03, addr, count);  // 功能码0x03: 读保持寄存器
    int rc = modbus_read_registers(ctx_, addr, count, out.data());
    bool success = (rc == count);
    // 只有在启用自动更新时才更新总线错误状态
    if (auto_update_bus_error_) {
        updateBusErrorStatus(success);
    }
    if (!success) {
        DEBUG_MODBUS_HOLDING_REG_LOG("从站[" << slave_id_ << "] <- 响应: 读保持寄存器失败 addr="<<addr<<" count="<<count<<" rc="<<rc);
    } else {
        print_modbus_read_response_hex(slave_id_, 0x03, out.data(), count, true);  // 功能码0x03: 读保持寄存器
        DEBUG_MODBUS_HOLDING_REG_LOG("从站[" << slave_id_ << "] <- 响应: 读保持寄存器成功 addr="<<addr<<" count="<<count<<" data=[");
        for (int i = 0; i < count; i++) {
            uint16_t raw_value = out[i];
            DEBUG_MODBUS_HOLDING_REG_LOG("  [" << i << "]=" << raw_value);
            
            // 保持寄存器通常是模拟量输出，按电压输入处理
            VoltageConverter::ChannelType channel_type = VoltageConverter::ChannelType::VOLTAGE_0_10V;
            std::string channel_name = "AO" + std::to_string(addr + i + 1);
            
            // 转换并显示电压值
            auto result = VoltageConverter::convertToVoltage(raw_value, channel_type, modbus_max_value_);
            std::string status = VoltageConverter::getChannelStatus(raw_value, channel_type, modbus_max_value_);
            DEBUG_MODBUS_VOLTAGE_CONV_LOG("    " << channel_name << ": " << VoltageConverter::formatResult(result, channel_type) 
                 << " [" << status << "]");
        }
        DEBUG_MODBUS_HOLDING_REG_LOG("]");
    }
    return success;
}

/**
 * @brief 批量读取线圈
 * @param addr 起始地址
 * @param count 读取数量
 * @param out 输出向量，会被调整为count大小
 * @return 读取成功返回true，失败返回false
 */
bool ModbusClient::readCoilRange(int addr, int count, std::vector<uint8_t>& out){
    out.resize(count);
    DEBUG_MODBUS_COIL_LOG("从站[" << slave_id_ << "] -> 发送: 读线圈 addr=" << addr << " count=" << count);
    print_modbus_read_request_hex(slave_id_, 0x01, addr, count);  // 功能码0x01: 读线圈
    int rc = modbus_read_bits(ctx_, addr, count, out.data());
    bool success = (rc == count);
    updateBusErrorStatus(success);
    if (!success) {
        DEBUG_MODBUS_COIL_LOG("从站[" << slave_id_ << "] <- 响应: 读线圈失败 addr="<<addr<<" count="<<count<<" rc="<<rc);
    } else {
        print_modbus_read_response_hex(slave_id_, 0x01, out.data(), count, false);  // 功能码0x01: 读线圈
        DEBUG_MODBUS_COIL_LOG("从站[" << slave_id_ << "] <- 响应: 读线圈成功 addr="<<addr<<" count="<<count);
    }
    return success;
}

bool ModbusClient::readDiscreteInputRange(int addr, int count, std::vector<uint8_t>& out){
    out.resize(count);
    DEBUG_MODBUS_DISCRETE_INPUT_LOG("从站[" << slave_id_ << "] -> 发送: 读离散输入 addr=" << addr << " count=" << count);
    print_modbus_read_request_hex(slave_id_, 0x02, addr, count);  // 功能码0x02: 读离散输入
    int rc = modbus_read_input_bits(ctx_, addr, count, out.data());
    bool success = (rc == count);
    updateBusErrorStatus(success);
    if (!success) {
        DEBUG_MODBUS_DISCRETE_INPUT_LOG("从站[" << slave_id_ << "] <- 响应: 读离散输入失败 addr="<<addr<<" count="<<count<<" rc="<<rc);
    } else {
        print_modbus_read_response_hex(slave_id_, 0x02, out.data(), count, false);  // 功能码0x02: 读离散输入
        DEBUG_MODBUS_DISCRETE_INPUT_LOG("从站[" << slave_id_ << "] <- 响应: 读离散输入成功 addr="<<addr<<" count="<<count<<" data=[");
        for (int i = 0; i < count; i++) {
            DEBUG_MODBUS_DISCRETE_INPUT_LOG("  [" << i << "]=" << (int)out[i]);
        }
        DEBUG_MODBUS_DISCRETE_INPUT_LOG("]");
    }
    return success;
}

bool ModbusClient::writeSingleCoil(int addr, bool value){
    DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] -> 发送: 写单个线圈 addr=" << addr << " value=" << (value ? "ON" : "OFF"));
    print_modbus_write_single_coil_hex(slave_id_, addr, value);
    int rc = modbus_write_bit(ctx_, addr, value ? 1 : 0);
    bool success = (rc == 1);
    updateBusErrorStatus(success);
    if (!success) {
        DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] <- 响应: 写单个线圈失败 addr="<<addr<<" value="<<(value?"ON":"OFF")<<" rc="<<rc);
    } else {
        uint16_t coil_value = value ? 0xFF00 : 0x0000;
        print_modbus_write_response_hex(slave_id_, 0x05, addr, coil_value);  // 功能码0x05: 写单个线圈
        DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] <- 响应: 写单个线圈成功 addr="<<addr<<" value="<<(value?"ON":"OFF"));
    }
    return success;
}

bool ModbusClient::writeMultipleCoils(int addr, const std::vector<bool>& values){
    if (values.empty()) {
        DEBUG_MODBUS_WARNING("写多个线圈时值向量为空");
        return true;
    }
    
    // 转换为uint8_t数组
    std::vector<uint8_t> coil_values(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        coil_values[i] = values[i] ? 1 : 0;
    }
    
    DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] -> 发送: 写多个线圈 addr=" << addr << " count=" << values.size());
    int rc = modbus_write_bits(ctx_, addr, values.size(), coil_values.data());
    bool success = (rc == static_cast<int>(values.size()));
    updateBusErrorStatus(success);
    if (!success) {
        DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] <- 响应: 写多个线圈失败 addr="<<addr<<" count="<<values.size()<<" rc="<<rc);
    } else {
        DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] <- 响应: 写多个线圈成功 addr="<<addr<<" count="<<values.size());
    }
    return success;
}

bool ModbusClient::writeSingleRegister(int addr, uint16_t value){
    DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] -> 发送: 写单个寄存器 addr=" << addr << " value=" << value);
    print_modbus_write_single_hex(slave_id_, 0x06, addr, value);  // 功能码0x06: 写单个寄存器
    int rc = modbus_write_register(ctx_, addr, value);
    bool success = (rc == 1);
    updateBusErrorStatus(success);
    if (!success) {
        DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] <- 响应: 写单个寄存器失败 addr="<<addr<<" value="<<value<<" rc="<<rc);
    } else {
        print_modbus_write_response_hex(slave_id_, 0x06, addr, value);  // 功能码0x06: 写单个寄存器
        DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] <- 响应: 写单个寄存器成功 addr="<<addr<<" value="<<value);
    }
    return success;
}

bool ModbusClient::writeMultipleRegisters(int addr, const std::vector<uint16_t>& values){
    if (values.empty()) {
        DEBUG_MODBUS_WARNING("写多个寄存器时值向量为空");
        return true;
    }
    
    DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] -> 发送: 写多个寄存器 addr=" << addr << " count=" << values.size() << " values=[");
    for (size_t i = 0; i < values.size() && i < 10; i++) {  // 最多显示前10个值
        DEBUG_MODBUS_WRITE_LOG("  [" << i << "]=" << values[i]);
    }
    if (values.size() > 10) {
        DEBUG_MODBUS_WRITE_LOG("  ... (total " << values.size() << " values)");
    }
    DEBUG_MODBUS_WRITE_LOG("]");
    int rc = modbus_write_registers(ctx_, addr, values.size(), values.data());
    bool success = (rc == static_cast<int>(values.size()));
    updateBusErrorStatus(success);
    if (!success) {
        DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] <- 响应: 写多个寄存器失败 addr="<<addr<<" count="<<values.size()<<" rc="<<rc);
    } else {
        DEBUG_MODBUS_WRITE_LOG("从站[" << slave_id_ << "] <- 响应: 写多个寄存器成功 addr="<<addr<<" count="<<values.size());
    }
    return success;
}

/**
 * @brief 读取单个输入寄存器
 * @param addr 寄存器地址
 * @param out 输出值
 * @return 读取成功返回true，失败返回false
 */
bool ModbusClient::readInput(int addr, uint16_t& out){
    std::vector<uint16_t> v;
    if (!readInputRange(addr,1,v)) return false;
    out = v[0]; return true;
}

/**
 * @brief 读取单个保持寄存器
 * @param addr 寄存器地址
 * @param out 输出值
 * @return 读取成功返回true，失败返回false
 */
bool ModbusClient::readHolding(int addr, uint16_t& out){
    std::vector<uint16_t> v;
    if (!readHoldingRange(addr,1,v)) return false;
    out = v[0]; return true;
}

/**
 * @brief 读取单个线圈
 * @param addr 线圈地址
 * @param out 输出值
 * @return 读取成功返回true，失败返回false
 */
bool ModbusClient::readCoil(int addr, uint8_t& out){
    std::vector<uint8_t> v;
    if (!readCoilRange(addr,1,v)) return false;
    out = v[0]; return true;
}

/**
 * @brief 读取单个离散量输入
 * @param addr 离散量输入地址
 * @param out 输出值
 * @return 读取成功返回true，失败返回false
 */
bool ModbusClient::readDiscreteInput(int addr, uint8_t& out){
    std::vector<uint8_t> v;
    if (!readDiscreteInputRange(addr,1,v)) return false;
    out = v[0]; return true;
}

/**
 * @brief 根据位源类型读取位值
 * @param s 位源描述
 * @return 位值（0或1），读取失败时返回0
 */
int ModbusClient::readBit(const RegBitSource& s){
    if (s.kind==ButtonSourceKind::Coil) {
        // 读取线圈值
        uint8_t v=0; if (readCoil(s.addr, v)) return v?1:0; else return 0;
    } else if (s.kind==ButtonSourceKind::InputRegisterBit) {
        // 读取输入寄存器的指定位
        uint16_t r=0; if (readInput(s.addr, r)) return ((r >> s.bit) & 1) ? 1:0; else return 0;
    } else {
        // 读取保持寄存器的指定位
        uint16_t r=0; if (readHolding(s.addr, r)) return ((r >> s.bit) & 1) ? 1:0; else return 0;
    }
}

/**
 * @brief 检查总线错误状态
 * @return 如果检测到总线错误返回true，否则返回false
 */
bool ModbusClient::isBusError() const {
    return bus_error_detected_;
}

/**
 * @brief 重置总线错误状态
 */
void ModbusClient::resetBusError() {
    consecutive_errors_ = 0;
    bus_error_detected_ = false;
}

/**
 * @brief 获取总线错误信息
 * @return 总线错误信息字符串
 */
std::string ModbusClient::getBusErrorInfo() const {
    if (!bus_error_detected_) {
        return "未检测到总线错误";
    }
    
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - first_error_time_);
    
    return "检测到总线错误: " + std::to_string(consecutive_errors_) + 
           " 次连续错误，持续 " + std::to_string(duration.count()) + 
           " 秒 (阈值: " + std::to_string(bus_error_threshold_) + "秒)";
}

/**
 * @brief 更新总线错误状态
 * @param success 当前操作是否成功
 */
void ModbusClient::updateBusErrorStatus(bool success) {
    auto now = std::chrono::steady_clock::now();
    
    if (!success) {
        // 操作失败，增加错误计数
        if (consecutive_errors_ == 0) {
            // 第一次错误，记录时间
            first_error_time_ = now;
        }
        consecutive_errors_++;
        
        // 检查是否超过阈值
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - first_error_time_);
        if (duration.count() >= bus_error_threshold_) {
            if (!bus_error_detected_) {
                // 只在第一次检测到总线错误时打印日志
                bus_error_detected_ = true;
                DEBUG_MODBUS_ERROR("检测到总线错误: " << consecutive_errors_ << " 次连续错误，持续 " 
                     << duration.count() << " 秒");
            } else {
                // 定期报告总线错误状态（每1秒报告一次）
                static auto last_report_time = std::chrono::steady_clock::now();
                auto time_since_last_report = std::chrono::duration_cast<std::chrono::seconds>(now - last_report_time);
                if (time_since_last_report.count() >= 1) {
                    DEBUG_MODBUS_ERROR("总线错误持续中: " << consecutive_errors_ << " 次连续错误，持续 " 
                         << duration.count() << " 秒");
                    last_report_time = now;
                }
            }
        }
    } else {
        // 操作成功，重置错误状态
        if (consecutive_errors_ > 0) {
            // 错误已清除，使用信息级别日志而不是错误级别
            // DEBUG_MODBUS_COMM_LOG("Bus error cleared after " << consecutive_errors_ << " consecutive errors, communication restored");
        }
        resetBusError();
    }
}

void ModbusClient::enableAutoReconnect(bool enable, int reconnect_interval_ms, int max_reconnect_attempts) {
    auto_reconnect_enabled_.store(enable);
    reconnect_interval_ms_.store(std::max(MODBUS_RECONNECT_MIN_INTERVAL_MS, reconnect_interval_ms)); // 最小间隔限制
    max_reconnect_attempts_.store(std::max(1, max_reconnect_attempts));
    
    DEBUG_MODBUS_COMM_LOG("自动重连 " << (enable ? "已启用" : "已禁用") 
                  << " (间隔: " << reconnect_interval_ms_.load() 
                  << "毫秒, 无限次尝试)");
}

bool ModbusClient::reconnect(bool is_bus_error) {
    if (!auto_reconnect_enabled_.load()) {
        DEBUG_MODBUS_ERROR("自动重连已禁用");
        return false;
    }
    
    // 对于总线错误，不检查USB设备存在性，直接尝试重连
    if (!is_bus_error) {
        if (!canAttemptReconnect()) {
            // 只在第一次被阻止时打印日志，避免重复打印
            static bool last_cannot_reconnect = false;
            if (!last_cannot_reconnect) {
                DEBUG_MODBUS_ERROR("此时无法尝试重连 (等待重连间隔)");
            }
            last_cannot_reconnect = true;
            return false;
        }
        
        // 重置状态，允许下次打印
        static bool last_cannot_reconnect = false;
        last_cannot_reconnect = false;
        
        // 检查设备是否仍然存在（仅对非总线错误）
        if (!checkDeviceExists()) {
            DEBUG_MODBUS_ERROR("设备 " << port_ << " 不存在，无法重连");
            connected_ = false;  // 设备不存在时更新连接状态
            updateReconnectStats(false);
            return false;
        }
    }
    
    std::string device_prefix = device_type_.empty() ? "" : ("[" + device_type_ + "] ");
    DEBUG_MODBUS_COMM_LOG(device_prefix << "正在尝试重连到 " << port_ << (is_bus_error ? " (总线错误恢复)" : ""));
    
    // 关闭当前连接
    close();
    
    // 重新创建上下文
    if (!recreateContext()) {
        DEBUG_MODBUS_ERROR("无法重新创建 Modbus 上下文");
        updateReconnectStats(false);
        return false;
    }
    
    // 尝试重新连接
    bool success = connect();
    updateReconnectStats(success);
    
    if (success) {
        DEBUG_MODBUS_COMM_LOG(device_prefix << "成功重连到 " << port_);
        connected_ = true;  // 确保连接状态立即更新
        if (connection_callback_) {
            connection_callback_(port_, true);
        }
    } else {
        DEBUG_MODBUS_ERROR("重连失败: " << port_);
        connected_ = false;  // 确保连接状态立即更新
        if (connection_callback_) {
            connection_callback_(port_, false);
        }
    }
    
    return success;
}

std::string ModbusClient::getConnectionStatus() const {
    std::ostringstream oss;
    oss << "端口: " << port_ << ", 已连接: " << (connected_ ? "是" : "否");
    
    if (ctx_) {
        oss << ", 上下文: 有效";
    } else {
        oss << ", 上下文: 无效";
    }
    
    if (bus_error_detected_) {
        oss << ", 总线错误: 是 (" << consecutive_errors_ << " 次连续错误)";
    } else {
        oss << ", 总线错误: 否";
    }
    
    if (auto_reconnect_enabled_.load()) {
        oss << ", 自动重连: 已启用 (尝试次数: " << reconnect_attempts_.load() 
            << ", 无限次)";
    } else {
        oss << ", 自动重连: 已禁用";
    }
    
    return oss.str();
}

void ModbusClient::setConnectionCallback(std::function<void(const std::string&, bool)> callback) {
    connection_callback_ = callback;
}

bool ModbusClient::checkDeviceExists() const {
    struct stat st;
    return stat(port_.c_str(), &st) == 0 && S_ISCHR(st.st_mode);
}

std::string ModbusClient::getReconnectStats() const {
    std::ostringstream oss;
    oss << "重连尝试次数: " << reconnect_attempts_.load() 
        << " (无限次)";
    
    if (reconnect_attempts_.load() > 0) {
        auto now = std::chrono::steady_clock::now();
        auto time_since_last_attempt = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - last_reconnect_attempt_).count();
        oss << ", 上次尝试: " << time_since_last_attempt << "毫秒前";
    }
    
    auto time_since_last_success = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - last_successful_operation_).count();
    oss << ", 上次成功: " << time_since_last_success << "秒前";
    
    return oss.str();
}

bool ModbusClient::recreateContext() {
    // 释放旧上下文
    if (ctx_) {
        modbus_free(ctx_);
        ctx_ = nullptr;
    }
    
    // 创建新上下文
    ctx_ = modbus_new_rtu(port_.c_str(), baud_, parity_, data_bits_, stop_bits_);
    if (!ctx_) {
        DEBUG_MODBUS_ERROR("无法重新创建 Modbus 上下文: " << modbus_strerror(errno));
        return false;
    }
    
    // 设置从站ID
    modbus_set_slave(ctx_, slave_id_);
    
    // 设置超时时间
    struct timeval tv{ .tv_sec = DEFAULT_TIMEOUT_MS / MILLISECONDS_PER_SECOND, .tv_usec = 0 };
    modbus_set_response_timeout(ctx_, tv.tv_sec, tv.tv_usec);
    modbus_set_byte_timeout(ctx_, tv.tv_sec, tv.tv_usec);
    
    DEBUG_MODBUS_COMM_LOG("Modbus 上下文已成功重新创建");
    return true;
}

bool ModbusClient::canAttemptReconnect() const {
    if (!auto_reconnect_enabled_.load()) {
        return false;
    }
    
    // 移除重连次数限制，持续尝试重连
    // if (reconnect_attempts_.load() >= max_reconnect_attempts_.load()) {
    //     return false;
    // }
    
    // 检查USB设备是否存在
    if (!checkDeviceExists()) {
        // 只在第一次检测到USB设备不可用时打印日志，避免重复刷屏
        static bool last_usb_unavailable = false;
        if (!last_usb_unavailable) {
            DEBUG_MODBUS_COMM_LOG("USB 设备不可用，等待设备重新连接...");
        }
        last_usb_unavailable = true;
        return false;
    }
    
    // 重置状态，允许下次打印
    static bool last_usb_unavailable = false;
    last_usb_unavailable = false;
    
    auto now = std::chrono::steady_clock::now();
    auto time_since_last_attempt = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - last_reconnect_attempt_).count();
    
    return time_since_last_attempt >= reconnect_interval_ms_.load();
}

void ModbusClient::updateReconnectStats(bool success) {
    auto now = std::chrono::steady_clock::now();
    last_reconnect_attempt_ = now;
    
    if (success) {
        reconnect_attempts_.store(0);
        last_successful_operation_ = now;
    } else {
        reconnect_attempts_.fetch_add(1);
    }
}

/**
 * @brief 设置Modbus从站ID（动态切换）
 * @param slave_id 从站ID
 * @return 设置成功返回true，失败返回false
 */
bool ModbusClient::setSlaveId(int slave_id) {
    if (!ctx_) {
        DEBUG_MODBUS_ERROR("Modbus 上下文未初始化，无法设置从站 ID");
        return false;
    }
    
    // 验证从站ID范围
    if (slave_id < 1 || slave_id > 247) {
        DEBUG_MODBUS_ERROR("无效的从站 ID: " << slave_id << " (必须在 1-247 范围内)");
        return false;
    }
    
    // 更新内部存储的从站ID
    slave_id_ = slave_id;
    
    // 设置Modbus上下文中的从站ID
    int rc = modbus_set_slave(ctx_, slave_id);
    if (rc == -1) {
        DEBUG_MODBUS_ERROR("无法设置从站 ID: " << modbus_strerror(errno));
        return false;
    }
    
    // DEBUG_MODBUS_COMM_LOG("Slave ID set to: " << slave_id);
    return true;
}
