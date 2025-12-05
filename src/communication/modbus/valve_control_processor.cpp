/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-01-XX XX:XX:XX
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-01-XX XX:XX:XX
 * @FilePath: src/communication/modbus/valve_control_processor.cpp
 * @Description: 阀控板Modbus处理器实现文件
 * 负责处理阀控板的Modbus通信，使用功能码03读取保持寄存器
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "valve_control_processor.h"
#include "utils.h"
#include "utils/thread_safe_logger.h"
#include "core/constants.h"
#include <algorithm>
#include <cmath>
#include <mutex>
#include <iomanip>
#include <sstream>

#ifdef ENABLE_ROS2
#include <sensor_msgs/msg/joy.hpp>
#endif

ValveControlProcessor::ValveControlProcessor(const std::string& port, int baud, char parity, int data_bits, int stop_bits, int slave_id, int poll_interval_ms)
    : poll_interval_ms_(poll_interval_ms) {
    status_.valve_status.resize(VALVE_COUNT, 0);
    last_valve_currents_.resize(VALVE_COUNT, -1); // 初始化为-1，确保第一次写入
    status_.valid = false;
#ifdef ENABLE_ROS2
    last_joystick_axes_.resize(VALVE_COUNT, 0.0f);
#endif
    
    // 创建独立的Modbus客户端
    modbus_client_ = std::make_unique<ModbusClient>(
        port, baud, parity, data_bits, stop_bits, slave_id, 2  // bus_error_threshold默认2秒
    );
    
    // 启用自动重连功能
    modbus_client_->enableAutoReconnect(true, MODBUS_AUTO_RECONNECT_INTERVAL_MS, MODBUS_AUTO_RECONNECT_MAX_ATTEMPTS);
}

bool ValveControlProcessor::initialize() {
    if (!modbus_client_) {
        DEBUG_VALVE_CONTROL_LOG("ValveControlProcessor: Modbus client not created");
        return false;
    }
    
    // 连接Modbus
    if (!modbus_client_->connect()) {
        DEBUG_VALVE_CONTROL_LOG("ValveControlProcessor: Initial connection failed, auto-reconnect enabled");
    } else {
        DEBUG_VALVE_CONTROL_LOG("ValveControlProcessor initialized and connected");
    }
    
    return true;
}

bool ValveControlProcessor::pollValveStatus() {
    // 检查连接状态，如果断开则只进行重连，不执行其他操作
    if (!isConnected()) {
        std::lock_guard<std::mutex> lock(status_mutex_);
        status_.valid = false;
        return false;
    }
    
    // 检查是否到了轮询时间
    auto now = std::chrono::steady_clock::now();
    auto time_since_last_poll = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - last_poll_time_).count();
    
    if (time_since_last_poll < poll_interval_ms_) {
        return status_.valid; // 返回上次的状态
    }
    
    last_poll_time_ = now;
    
    // 更新心跳值（每次发送不同的心跳值，使用时间戳的低16位）
    static uint16_t heartbeat_counter = 0;
    heartbeat_counter++;
    setHeartbeat(heartbeat_counter);
    
    // 读取地址0-132的所有保持寄存器（共133个寄存器）
    // 使用功能码03（读保持寄存器）
    // 注意：Modbus RTU通常一次最多读取125个寄存器，所以分两次读取
    constexpr int REG_START = 0;
    constexpr int REG_COUNT = 133;  // 0-132共133个寄存器
    constexpr int FIRST_BATCH_COUNT = 125;  // 第一次读取0-124
    constexpr int SECOND_BATCH_START = 125;  // 第二次读取125-132
    constexpr int SECOND_BATCH_COUNT = 8;    // 第二次读取8个寄存器
    
    std::vector<uint16_t> all_regs;
    all_regs.resize(REG_COUNT, 0);  // 初始化为0
    
    // 第一次读取：地址0-124
    std::vector<uint16_t> first_batch;
    first_batch.resize(FIRST_BATCH_COUNT);
    if (!modbus_client_ || !modbus_client_->readHoldingRange(REG_START, FIRST_BATCH_COUNT, first_batch)) {
        std::lock_guard<std::mutex> lock(status_mutex_);
        status_.valid = false;
        last_error_ = "Failed to read holding registers (addr=0-124) using function code 03";
        DEBUG_VALVE_CONTROL_LOG("Error: " + last_error_);
        return false;
    }
    
    // 复制第一批数据
    for (int i = 0; i < FIRST_BATCH_COUNT && i < static_cast<int>(all_regs.size()); ++i) {
        all_regs[i] = first_batch[i];
    }
    
    // 第二次读取：地址125-132
    std::vector<uint16_t> second_batch;
    second_batch.resize(SECOND_BATCH_COUNT);
    if (!modbus_client_ || !modbus_client_->readHoldingRange(SECOND_BATCH_START, SECOND_BATCH_COUNT, second_batch)) {
        std::lock_guard<std::mutex> lock(status_mutex_);
        status_.valid = false;
        last_error_ = "Failed to read holding registers (addr=125-132) using function code 03";
        DEBUG_VALVE_CONTROL_LOG("Error: " + last_error_);
        return false;
    }
    
    // 复制第二批数据
    for (int i = 0; i < SECOND_BATCH_COUNT && (SECOND_BATCH_START + i) < static_cast<int>(all_regs.size()); ++i) {
        all_regs[SECOND_BATCH_START + i] = second_batch[i];
    }
    
    // 验证读取的数据大小
    if (all_regs.size() < REG_COUNT) {
        std::lock_guard<std::mutex> lock(status_mutex_);
        status_.valid = false;
        last_error_ = "Incomplete data read: expected " + std::to_string(REG_COUNT) 
                     + " registers, got " + std::to_string(all_regs.size());
        DEBUG_VALVE_CONTROL_LOG("Error: " + last_error_);
        return false;
    }
    
    // 从读取的数据中提取需要的值（带边界检查）
    uint16_t accumulator = (ACCUMULATOR_REG < all_regs.size()) ? all_regs[ACCUMULATOR_REG] : 0;  // 地址0
    uint16_t heartbeat = (HEARTBEAT_REG < all_regs.size()) ? all_regs[HEARTBEAT_REG] : 0;       // 地址63
    
    // 提取阀状态寄存器（地址56-60，共5个寄存器）
    std::vector<uint16_t> status_regs;
    status_regs.resize(STATUS_REG_COUNT);
    for (int i = 0; i < STATUS_REG_COUNT; ++i) {
        int reg_addr = STATUS_REG_START + i;
        if (reg_addr < static_cast<int>(all_regs.size())) {
            status_regs[i] = all_regs[reg_addr];
        } else {
            status_regs[i] = 0;  // 超出范围时设为0
            DEBUG_VALVE_CONTROL_LOG("Warning: Register address " << reg_addr << " out of range");
        }
    }
    
    // 更新状态
    {
        std::lock_guard<std::mutex> lock(status_mutex_);
        status_.accumulator = accumulator;
        status_.heartbeat = heartbeat;
        status_.last_update = now;
        parseValveStatus(status_regs);
        status_.valid = true;
        last_error_.clear();
    }
    
    // 打印接收到的状态数据（详细信息）
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("========== Valve Control Status ==========");
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("Accumulator (addr 0): " << accumulator);
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("Heartbeat (addr 63): " << heartbeat);
    
    // 打印所有寄存器数据（地址0-132），使用紧凑格式
    DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG("--- All Registers (0-132) ---");
    
    // 1. 系统寄存器 (0-1) - 单行显示
    DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG("  [00-01] 累加器=" << all_regs[0] 
        << "(每1s+1) PCB状态=" << all_regs[1] << "(预留)");
    
    // 2. 阀电流值 (2-21) - 合并显示，只显示非零值
    std::ostringstream valve_current_oss;
    valve_current_oss << "  [02-21] 阀电流值(单位:mA×10): ";
    bool has_current = false;
    for (int i = 2; i <= 21 && i < static_cast<int>(all_regs.size()); ++i) {
        if (all_regs[i] != 0) {
            if (has_current) valve_current_oss << ", ";
            int valve_base = (i < 12) ? ((i-2)*2+1) : ((i-12)*2+21);
            valve_current_oss << "V" << valve_base << "-" << (valve_base+1) << "=" << all_regs[i];
            has_current = true;
        }
    }
    if (!has_current) valve_current_oss << "全部为0";
    DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG(valve_current_oss.str());
    
    // 3. 传感器数据 (22-45) - 合并显示，只显示非零值
    std::ostringstream sensor_oss;
    sensor_oss << "  [22-45] 传感器(电压型:mv, 电流型:1uA/0.01mA): ";
    bool has_sensor = false;
    for (int i = 22; i <= 45 && i < static_cast<int>(all_regs.size()); ++i) {
        if (all_regs[i] != 0) {
            if (has_sensor) sensor_oss << ", ";
            sensor_oss << "S" << (i-22+1) << "=" << all_regs[i];
            has_sensor = true;
        }
    }
    if (!has_sensor) sensor_oss << "全部为0";
    DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG(sensor_oss.str());
    
    // 4. 编码器数据 (46-49) - 合并显示
    if (49 < static_cast<int>(all_regs.size())) {
        int32_t enc1 = static_cast<int32_t>(all_regs[46]) | (static_cast<int32_t>(all_regs[47]) << 16);
        int32_t enc2 = static_cast<int32_t>(all_regs[48]) | (static_cast<int32_t>(all_regs[49]) << 16);
        DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG("  [46-49] 编码器1=" << enc1 
            << " 编码器2=" << enc2 << " (32位有符号,范围:-2147483647~2147483647)");
    }
    
    // 5. 接近开关 (50) - 单行显示
    if (50 < static_cast<int>(all_regs.size())) {
        std::ostringstream prox_oss;
        prox_oss << "  [50] 接近开关(0=未触发,1=触发): ";
        for (int i = 0; i < 6; ++i) {
            if (i > 0) prox_oss << " ";
            prox_oss << "S" << (i+1) << "=" << ((all_regs[50] >> i) & 0x01);
        }
        DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG(prox_oss.str());
    }
    
    // 6. 湿度/温度/板载传感器 (51-55) - 单行显示
    if (55 < static_cast<int>(all_regs.size())) {
        DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG("  [51-55] 湿度=" << all_regs[51] << "mV(正常约3300) "
            << "温度1=" << all_regs[52] << "(0.1°C) 温度2=" << all_regs[53] << "(0.1°C) "
            << "板载温度=" << all_regs[54] << "(0.1°C) 板载电压=" << all_regs[55] << "mV");
    }
    
    // 7. 阀状态 (56-60) - 合并显示，只显示异常状态
    std::ostringstream valve_status_oss;
    valve_status_oss << "  [56-60] 阀状态(0=正常,2=报警,3=离线): ";
    bool has_abnormal = false;
    for (int i = 56; i <= 60 && i < static_cast<int>(all_regs.size()); ++i) {
        if (all_regs[i] != 0 && all_regs[i] != 0xFFFF) {
            if (has_abnormal) valve_status_oss << ", ";
            valve_status_oss << "V" << ((i-56)*8+1) << "-" << ((i-56)*8+8) << "=0x" 
                << std::hex << all_regs[i] << std::dec;
            has_abnormal = true;
        }
    }
    if (!has_abnormal) {
        if (all_regs[56] == 0xFFFF) valve_status_oss << "全部离线(0xFFFF)";
        else valve_status_oss << "全部正常";
    }
    DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG(valve_status_oss.str());
    
    // 8. 传感器状态 (61-62) - 合并显示，只显示报警
    if (62 < static_cast<int>(all_regs.size())) {
        std::ostringstream sensor_status_oss;
        sensor_status_oss << "  [61-62] 传感器状态(0=正常,1=报警): ";
        bool has_alarm = false;
        for (int i = 61; i <= 62; ++i) {
            if (all_regs[i] != 0) {
                if (has_alarm) sensor_status_oss << ", ";
                sensor_status_oss << "S" << ((i-61)*16+1) << "-" << ((i-61)*16+16) << "=0x" 
                    << std::hex << all_regs[i] << std::dec;
                has_alarm = true;
            }
        }
        if (!has_alarm) sensor_status_oss << "全部正常";
        DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG(sensor_status_oss.str());
    }
    
    // 9. 心跳 (63) - 单行显示
    if (63 < static_cast<int>(all_regs.size())) {
        DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG("  [63] 心跳=" << all_regs[63] 
            << "(每次发送不同的心跳值,2秒内未收到新值则输出关闭)");
    }
    
    // 10. 阀电流值设定 (64-103) - 合并显示，只显示非零值
    std::ostringstream valve_set_oss;
    valve_set_oss << "  [64-103] 阀电流设定(0~30000对应0mA~1500mA,<0时调节至0): ";
    bool has_set = false;
    for (int i = 64; i <= 103 && i < static_cast<int>(all_regs.size()); ++i) {
        // 转换为有符号整数（保持二进制表示）
        int16_t signed_val = static_cast<int16_t>(all_regs[i]);
        if (signed_val != 0) {
            if (has_set) valve_set_oss << ", ";
            valve_set_oss << "V" << (i-64+1) << "=" << signed_val;
            has_set = true;
        }
    }
    if (!has_set) valve_set_oss << "全部为0";
    DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG(valve_set_oss.str());
    
    // 11. 传感器电源开关 (104-105) - 合并显示
    if (105 < static_cast<int>(all_regs.size())) {
        DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG("  [104-105] 传感器电源(0=关,1=开): S1-16=0x" 
            << std::hex << all_regs[104] << std::dec << " S17-32=0x" 
            << std::hex << all_regs[105] << std::dec);
    }
    
    // 12. 传感器配置 (106-108) - 合并显示
    if (108 < static_cast<int>(all_regs.size())) {
        DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG("  [106-108] 传感器配置(00=电压型,01=电流型): S1-8=0x" 
            << std::hex << all_regs[106] << std::dec << " S9-16=0x" 
            << std::hex << all_regs[107] << std::dec << " S17-24=0x" 
            << std::hex << all_regs[108] << std::dec);
    }
    
    // 13. 阀频率设定 (109-128) - 合并显示，只显示非零值
    std::ostringstream freq_oss;
    freq_oss << "  [109-128] 阀频率设定(输入11~250,实际输出110~2500Hz): ";
    bool has_freq = false;
    for (int i = 109; i <= 128 && i < static_cast<int>(all_regs.size()); ++i) {
        if (all_regs[i] != 0) {
            if (has_freq) freq_oss << ", ";
            freq_oss << "V" << ((i-109)*2+1) << "-" << ((i-109)*2+2) << "=" << all_regs[i];
            has_freq = true;
        }
    }
    if (!has_freq) freq_oss << "全部为0";
    DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG(freq_oss.str());
    
    // 14-17. 其他寄存器 (129-132) - 单行显示
    if (132 < static_cast<int>(all_regs.size())) {
        std::ostringstream misc_oss;
        misc_oss << "  [129-132] ";
        if (129 < static_cast<int>(all_regs.size())) {
            misc_oss << "编码器清零(写入1要求清除)=0x" << std::hex << all_regs[129] << std::dec << " ";
        }
        if (130 < static_cast<int>(all_regs.size())) {
            misc_oss << "Modbus地址=" << all_regs[130] << " ";
        }
        if (131 < static_cast<int>(all_regs.size())) {
            std::string baud_desc = "未知";
            switch(all_regs[131]) {
                case 1: baud_desc = "9600bps"; break;
                case 2: baud_desc = "19200bps(默认)"; break;
                case 3: baud_desc = "38400bps"; break;
                case 4: baud_desc = "57600bps"; break;
                case 5: baud_desc = "115200bps"; break;
            }
            misc_oss << "波特率=" << baud_desc << " ";
        }
        if (132 < static_cast<int>(all_regs.size())) {
            std::string reset_desc = "未知";
            if (all_regs[132] == 0x01) reset_desc = "RS485";
            else if (all_regs[132] == 0x02) reset_desc = "RS422";
            else if (all_regs[132] == 0x10) reset_desc = "复位";
            misc_oss << "复位(01=RS485,02=RS422,0x10=复位)=" << reset_desc;
        }
        DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG(misc_oss.str());
    }
    
    // 打印关键寄存器说明
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("--- Key Registers ---");
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("  [00] Accumulator: " << all_regs[0] << " (每1s+1，用于判断故障)");
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("  [01] PCB Status: " << all_regs[1] << " (预留)");
    if (all_regs.size() > 2) {
        DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("  [02] Valve 1-2 Current: " << all_regs[2] << " (低8位=阀1, 高8位=阀2, 单位: mA×10)");
    }
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("  [56] Valve 1-8 Status: " << all_regs[56] << " (0x" << std::hex << all_regs[56] << std::dec << ")");
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("  [57] Valve 9-16 Status: " << all_regs[57] << " (0x" << std::hex << all_regs[57] << std::dec << ")");
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("  [58] Valve 17-24 Status: " << all_regs[58] << " (0x" << std::hex << all_regs[58] << std::dec << ")");
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("  [59] Valve 25-32 Status: " << all_regs[59] << " (0x" << std::hex << all_regs[59] << std::dec << ")");
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("  [60] Valve 33-40 Status: " << all_regs[60] << " (0x" << std::hex << all_regs[60] << std::dec << ")");
    DEBUG_VALVE_CONTROL_STATUS_KEY_LOG("  [63] Heartbeat: " << all_regs[63]);
    
    // 打印阀电流值（地址2-21，共20个寄存器，对应40个阀）
    DEBUG_VALVE_CONTROL_STATUS_CURRENT_LOG("--- Valve Current Values (单位: mA×10, 0x64=1000mA) ---");
    for (int reg_idx = 2; reg_idx <= 21 && reg_idx < static_cast<int>(all_regs.size()); ++reg_idx) {
        uint16_t reg_val = all_regs[reg_idx];
        int valve_base = (reg_idx - 2) * 2 + 1;  // 阀编号从1开始
        
        // 前20个阀：低8位=阀1，高8位=阀2
        // 后20个阀（地址12-21）：高8位=阀1，低8位=阀2
        uint8_t valve1_current, valve2_current;
        if (reg_idx < 12) {
            // 地址2-11：低8位=阀1，高8位=阀2
            valve1_current = reg_val & 0xFF;
            valve2_current = (reg_val >> 8) & 0xFF;
        } else {
            // 地址12-21：高8位=阀1，低8位=阀2
            valve1_current = (reg_val >> 8) & 0xFF;
            valve2_current = reg_val & 0xFF;
        }
        
        std::ostringstream current_oss;
        current_oss << "  [" << std::setw(2) << std::setfill('0') << reg_idx << "] Valves " 
                    << std::setw(2) << std::setfill('0') << valve_base 
                    << "-" << std::setw(2) << std::setfill('0') << (valve_base + 1) << ": "
                    << "V" << std::setw(2) << std::setfill('0') << valve_base << "=" 
                    << std::setw(3) << static_cast<int>(valve1_current) << "mA  "
                    << "V" << std::setw(2) << std::setfill('0') << (valve_base + 1) << "=" 
                    << std::setw(3) << static_cast<int>(valve2_current) << "mA";
        DEBUG_VALVE_CONTROL_STATUS_CURRENT_LOG(current_oss.str());
    }
    
    // 打印传感器数据（地址22-45，共24个传感器）
    DEBUG_VALVE_CONTROL_STATUS_SENSOR_LOG("--- Sensor Data (地址22-45) ---");
    for (int reg_idx = 22; reg_idx <= 45 && reg_idx < static_cast<int>(all_regs.size()); ++reg_idx) {
        uint16_t sensor_val = all_regs[reg_idx];
        int sensor_num = reg_idx - 22 + 1;
        
        std::ostringstream sensor_detail_oss;
        sensor_detail_oss << "  [" << std::setw(2) << std::setfill('0') << reg_idx << "] Sensor " 
                   << std::setw(2) << std::setfill('0') << sensor_num << ": " << sensor_val;
        
        // 前12个传感器（地址22-33）：电压型0~10000表示0~10V(单位mv)，电流型0~20000表示0-20mA(单位1uA)
        // 后12个传感器（地址34-45）：电压型0~10000表示0~10V(单位mv)，电流型0~2000表示0-20mA(单位0.01mA)
        if (sensor_num <= 12) {
            sensor_detail_oss << " (电压型: 0~10000=0~10V单位mv, 电流型: 0~20000=0~20mA单位1uA)";
        } else {
            sensor_detail_oss << " (电压型: 0~10000=0~10V单位mv, 电流型: 0~2000=0~20mA单位0.01mA)";
        }
        DEBUG_VALVE_CONTROL_STATUS_SENSOR_LOG(sensor_detail_oss.str());
    }
    
    // 打印编码器数据（地址46-49，2个编码器，每个32位有符号）
    DEBUG_VALVE_CONTROL_STATUS_ENCODER_LOG("--- Encoder Count Values (32位有符号, 范围: -2147483647 到 2147483647) ---");
    for (int enc_idx = 0; enc_idx < 2; ++enc_idx) {
        int reg_low = 46 + enc_idx * 2;
        int reg_high = reg_low + 1;
        
        if (reg_high < static_cast<int>(all_regs.size())) {
            // 组合两个16位寄存器为32位有符号整数（低16位在前）
            int32_t encoder_value = static_cast<int32_t>(all_regs[reg_low]) | 
                                   (static_cast<int32_t>(all_regs[reg_high]) << 16);
            
            std::ostringstream enc_oss;
            enc_oss << "  [" << std::setw(2) << std::setfill('0') << reg_low << "-" 
                    << std::setw(2) << std::setfill('0') << reg_high << "] Encoder " 
                    << (enc_idx + 1) << ": " << encoder_value;
            DEBUG_VALVE_CONTROL_STATUS_ENCODER_LOG(enc_oss.str());
        }
    }
    
    // 打印接近开关状态（地址50，6个开关）
    if (50 < static_cast<int>(all_regs.size())) {
        uint16_t proximity_val = all_regs[50];
        std::ostringstream prox_oss;
        prox_oss << "--- Proximity Switch Status (地址50, 0=未触发, 1=触发) ---";
        DEBUG_VALVE_CONTROL_STATUS_PROXIMITY_LOG(prox_oss.str());
        
        std::ostringstream prox_detail_oss;
        prox_detail_oss << "  [50] Switches 1-6: ";
        for (int i = 0; i < 6; ++i) {
            if (i > 0) prox_detail_oss << "  ";
            int switch_state = (proximity_val >> i) & 0x01;
            prox_detail_oss << "S" << (i + 1) << "=" << switch_state;
        }
        DEBUG_VALVE_CONTROL_STATUS_PROXIMITY_LOG(prox_detail_oss.str());
    }
    
    // 打印湿度、温度、板载传感器数据（地址51-55）
    DEBUG_VALVE_CONTROL_STATUS_ENV_LOG("--- Humidity/Temperature/Board Sensors (地址51-55) ---");
    if (51 < static_cast<int>(all_regs.size())) {
        DEBUG_VALVE_CONTROL_STATUS_ENV_LOG("  [51] Humidity Sensor 1 Voltage: " << all_regs[51] << " (单位: 1mV, 正常约3300, 越低代表进水越严重)");
    }
    if (52 < static_cast<int>(all_regs.size())) {
        DEBUG_VALVE_CONTROL_STATUS_ENV_LOG("  [52] Temperature Sensor 1: " << all_regs[52] << " (单位: 0.1℃)");
    }
    if (53 < static_cast<int>(all_regs.size())) {
        DEBUG_VALVE_CONTROL_STATUS_ENV_LOG("  [53] Temperature Sensor 2: " << all_regs[53] << " (单位: 0.1℃)");
    }
    if (54 < static_cast<int>(all_regs.size())) {
        DEBUG_VALVE_CONTROL_STATUS_ENV_LOG("  [54] Board Temperature Sensor: " << all_regs[54] << " (单位: 0.1℃)");
    }
    if (55 < static_cast<int>(all_regs.size())) {
        DEBUG_VALVE_CONTROL_STATUS_ENV_LOG("  [55] Board Voltage Sensor: " << all_regs[55] << " (单位: 1mV)");
    }
    
    // 打印传感器状态（地址61-62）
    DEBUG_VALVE_CONTROL_STATUS_SENSOR_STATUS_LOG("--- Sensor Status (地址61-62, 0=正常, 1=报警) ---");
    if (61 < static_cast<int>(all_regs.size())) {
        uint16_t sensor_status_1 = all_regs[61];
        std::ostringstream ss_oss;
        ss_oss << "  [61] Sensors 1-16: ";
        for (int i = 0; i < 16; ++i) {
            if (i > 0 && i % 8 == 0) ss_oss << "\n                    ";
            int status = (sensor_status_1 >> i) & 0x01;
            ss_oss << "S" << std::setw(2) << std::setfill('0') << (i + 1) << "=" << status << "  ";
        }
        DEBUG_VALVE_CONTROL_STATUS_SENSOR_STATUS_LOG(ss_oss.str());
    }
    if (62 < static_cast<int>(all_regs.size())) {
        uint16_t sensor_status_2 = all_regs[62];
        std::ostringstream ss_oss;
        ss_oss << "  [62] Sensors 17-24: ";
        for (int i = 0; i < 8; ++i) {
            if (i > 0) ss_oss << "  ";
            int status = (sensor_status_2 >> i) & 0x01;
            ss_oss << "S" << std::setw(2) << std::setfill('0') << (i + 17) << "=" << status;
        }
        DEBUG_VALVE_CONTROL_STATUS_SENSOR_STATUS_LOG(ss_oss.str());
    }
    
    // 打印所有阀的状态（详细），按组显示
    DEBUG_VALVE_CONTROL_STATUS_VALVE_STATUS_LOG("--- Valve Status (0=正常, 2=报警, 3=离线) ---");
    for (int group = 0; group < 5; ++group) {
        std::ostringstream group_oss;
        int start_valve = group * 8;
        group_oss << "  Valves " << std::setw(2) << std::setfill('0') << (start_valve + 1) 
                  << "-" << std::setw(2) << std::setfill('0') << (start_valve + 8) << ": ";
        for (int i = 0; i < 8; ++i) {
            int valve_idx = start_valve + i;
            if (i > 0) group_oss << "  ";
            group_oss << "V" << std::setw(2) << std::setfill('0') << (valve_idx + 1) << "=" << status_.valve_status[valve_idx];
        }
        DEBUG_VALVE_CONTROL_STATUS_VALVE_STATUS_LOG(group_oss.str());
    }
    
    return true;
}

ValveControlStatus ValveControlProcessor::getStatus() const {
    std::lock_guard<std::mutex> lock(status_mutex_);
    return status_;
}

bool ValveControlProcessor::setValveCurrent(int valve_index, double joystick_value) {
    // 检查连接状态，如果断开则不执行写入操作
    if (!isConnected()) {
        return false;
    }
    
    if (valve_index < 0 || valve_index >= VALVE_COUNT) {
        last_error_ = "Invalid valve index: " + std::to_string(valve_index);
        DEBUG_VALVE_CONTROL_LOG("Error: " + last_error_);
        return false;
    }
    
    // 将摇杆值转换为电流值
    int16_t current_value = joystickToCurrent(joystick_value);
    
    // 检查是否需要写入（避免重复写入相同值）
    if (last_valve_currents_[valve_index] == current_value) {
        return true; // 值未变化，跳过写入
    }
    
    // 计算寄存器地址（地址64开始，对应阀1）
    int reg_addr = CURRENT_SET_START + valve_index;
    
    // 写入保持寄存器（16位有符号整数）
    // 注意：Modbus的writeSingleRegister接受uint16_t，但我们需要写入有符号值
    // 使用reinterpret_cast将int16_t按位转换为uint16_t（保持二进制表示）
    uint16_t reg_value = *reinterpret_cast<uint16_t*>(&current_value);
    
    if (!modbus_client_ || !modbus_client_->writeSingleRegister(reg_addr, reg_value)) {
        last_error_ = "Failed to write valve current for valve " + std::to_string(valve_index + 1);
        DEBUG_VALVE_CONTROL_WRITE_LOG("Error: " + last_error_);
        return false;
    }
    
    // 更新缓存
    last_valve_currents_[valve_index] = current_value;
    
    DEBUG_VALVE_CONTROL_WRITE_LOG("Set valve " << (valve_index + 1) 
                                  << " to " << current_value 
                                  << " (joystick=" << std::fixed << std::setprecision(3) << joystick_value << ")");
    
    return true;
}

bool ValveControlProcessor::setValveCurrents(const std::vector<double>& joystick_values) {
    bool all_success = true;
    
    for (size_t i = 0; i < joystick_values.size() && i < static_cast<size_t>(VALVE_COUNT); ++i) {
        if (!setValveCurrent(static_cast<int>(i), joystick_values[i])) {
            all_success = false;
        }
    }
    
    return all_success;
}

bool ValveControlProcessor::setHeartbeat(uint16_t heartbeat_value) {
    // 检查连接状态，如果断开则不执行写入操作
    if (!isConnected()) {
        return false;
    }
    
    if (!modbus_client_ || !modbus_client_->writeSingleRegister(HEARTBEAT_REG, heartbeat_value)) {
        last_error_ = "Failed to write heartbeat register";
        DEBUG_VALVE_CONTROL_WRITE_LOG("Error: " + last_error_);
        return false;
    }
    
    // DEBUG_VALVE_CONTROL_WRITE_LOG("Set heartbeat to " << heartbeat_value);
    return true;
}

bool ValveControlProcessor::isConnected() const {
    return modbus_client_ && modbus_client_->ok();
}

std::string ValveControlProcessor::getLastError() const {
    return last_error_;
}

bool ValveControlProcessor::reconnect() {
    if (!modbus_client_) {
        DEBUG_VALVE_CONTROL_LOG("ValveControlProcessor: Modbus client not created, cannot reconnect");
        return false;
    }
    
    if (!modbus_client_->isAutoReconnectEnabled()) {
        DEBUG_VALVE_CONTROL_LOG("ValveControlProcessor: Auto-reconnect is disabled");
        return false;
    }
    
    DEBUG_VALVE_CONTROL_LOG("ValveControlProcessor: Triggering reconnection...");
    bool success = modbus_client_->reconnect();
    
    if (success) {
        DEBUG_VALVE_CONTROL_LOG("ValveControlProcessor: Reconnection successful");
    } else {
        DEBUG_VALVE_CONTROL_LOG("ValveControlProcessor: Reconnection failed, will retry automatically");
    }
    
    return success;
}

void ValveControlProcessor::close() {
    if (modbus_client_) {
        DEBUG_VALVE_CONTROL_LOG("ValveControlProcessor: Closing connection...");
        modbus_client_->close();
    }
}

int16_t ValveControlProcessor::joystickToCurrent(double joystick_value) const {
    // 限制摇杆值范围（归一化值：0.0 到 1.0，因为已经拆分成正值）
    // 注意：这里接收的已经是正值（0.0到1.0）
    joystick_value = std::clamp(joystick_value, 0.0, 1.0);
    
    // 将归一化的joystick值转换为电流值
    // 根据文档：0~30000对应0mA~1500mA
    // 0.0 -> 0, 0.5 -> 15000, 1.0 -> 30000
    constexpr int SCALE_FACTOR = 30000;
    int16_t current = static_cast<int16_t>(std::round(joystick_value * SCALE_FACTOR));
    
    // 限制在有效范围内（0 到 30000，对应0mA~1500mA）
    return std::clamp(current, CURRENT_MIN, CURRENT_MAX);
}

void ValveControlProcessor::parseValveStatus(const std::vector<uint16_t>& status_regs) {
    // 每个寄存器包含8个阀的状态（每2bit表示1个阀的状态）
    // 0表示正常状态，2表示报警状态（短路或开路），3表示离线状态
    for (int reg_idx = 0; reg_idx < STATUS_REG_COUNT && reg_idx < static_cast<int>(status_regs.size()); ++reg_idx) {
        uint16_t reg_value = status_regs[reg_idx];
        
        // 每个寄存器包含8个阀的状态
        for (int valve_in_reg = 0; valve_in_reg < 8; ++valve_in_reg) {
            int valve_index = reg_idx * 8 + valve_in_reg;
            if (valve_index >= VALVE_COUNT) {
                break;
            }
            
            // 提取2bit状态（BIT0和BIT1表示第1个阀，BIT2和BIT3表示第2个阀...）
            int bit_offset = valve_in_reg * 2;
            uint16_t valve_status = (reg_value >> bit_offset) & 0x03;
            
            status_.valve_status[valve_index] = valve_status;
        }
    }
}

void ValveControlProcessor::printAllRegistersDetailed(const std::vector<uint16_t>& all_regs) const {
    // 根据文档说明打印每个寄存器的详细信息
    for (int addr = 0; addr < static_cast<int>(all_regs.size()) && addr <= 132; ++addr) {
        uint16_t value = all_regs[addr];
        std::ostringstream reg_msg;
        reg_msg << "  [" << std::setw(3) << std::setfill('0') << addr << "] ";
        
        // 根据地址打印寄存器名称和说明
        if (addr == 0) {
            reg_msg << "累加寄存器: " << value << " (每1s+1，用于判断故障)";
        } else if (addr == 1) {
            reg_msg << "PCB状态: " << value << " (预留)";
        } else if (addr >= 2 && addr <= 11) {
            // 阀1-20电流值（地址2-11）
            int valve_base = (addr - 2) * 2 + 1;
            uint8_t valve1 = value & 0xFF;
            uint8_t valve2 = (value >> 8) & 0xFF;
            reg_msg << "阀" << valve_base << "-" << (valve_base + 1) << "电流值: "
                << "V" << valve_base << "=" << static_cast<int>(valve1) << "mA×10, "
                << "V" << (valve_base + 1) << "=" << static_cast<int>(valve2) << "mA×10 "
                << "(低8位=阀" << valve_base << ", 高8位=阀" << (valve_base + 1) << ")";
        } else if (addr >= 12 && addr <= 21) {
            // 阀21-40电流值（地址12-21）
            int valve_base = (addr - 12) * 2 + 21;
            uint8_t valve1 = (value >> 8) & 0xFF;
            uint8_t valve2 = value & 0xFF;
            reg_msg << "阀" << valve_base << "-" << (valve_base + 1) << "电流值: "
                << "V" << valve_base << "=" << static_cast<int>(valve1) << "mA×10, "
                << "V" << (valve_base + 1) << "=" << static_cast<int>(valve2) << "mA×10 "
                << "(高8位=阀" << valve_base << ", 低8位=阀" << (valve_base + 1) << ")";
        } else if (addr >= 22 && addr <= 45) {
            // 传感器1-24数据（地址22-45）
            int sensor_num = addr - 22 + 1;
            reg_msg << "传感器" << sensor_num << "数据: " << value;
            if (sensor_num <= 12) {
                reg_msg << " (电压型: 0~10000=0~10V单位mv, 电流型: 0~20000=0~20mA单位1uA)";
            } else {
                reg_msg << " (电压型: 0~10000=0~10V单位mv, 电流型: 0~2000=0~20mA单位0.01mA)";
            }
        } else if (addr == 46) {
            // 编码器1计数值（地址46-47，32位有符号）
            if (47 < static_cast<int>(all_regs.size())) {
                int32_t enc_value = static_cast<int32_t>(all_regs[46]) | 
                                   (static_cast<int32_t>(all_regs[47]) << 16);
                reg_msg << "编码器1计数值: " << enc_value 
                    << " (32位有符号, 范围: -2147483647 到 2147483647) [低16位=" 
                    << all_regs[46] << ", 高16位=" << all_regs[47] << "]";
            } else {
                reg_msg << "编码器1计数值(低16位): " << value << " (数据不完整)";
            }
        } else if (addr == 47) {
            reg_msg << "编码器1计数值(高16位): " << value << " (与地址46组合为32位有符号整数)";
        } else if (addr == 48) {
            // 编码器2计数值（地址48-49，32位有符号）
            if (49 < static_cast<int>(all_regs.size())) {
                int32_t enc_value = static_cast<int32_t>(all_regs[48]) | 
                                   (static_cast<int32_t>(all_regs[49]) << 16);
                reg_msg << "编码器2计数值: " << enc_value 
                    << " (32位有符号, 范围: -2147483647 到 2147483647) [低16位=" 
                    << all_regs[48] << ", 高16位=" << all_regs[49] << "]";
            } else {
                reg_msg << "编码器2计数值(低16位): " << value << " (数据不完整)";
            }
        } else if (addr == 49) {
            reg_msg << "编码器2计数值(高16位): " << value << " (与地址48组合为32位有符号整数)";
        } else if (addr == 50) {
            // 接近开关6~1的状态
            reg_msg << "接近开关1-6状态: ";
            for (int i = 0; i < 6; ++i) {
                if (i > 0) reg_msg << ", ";
                int switch_state = (value >> i) & 0x01;
                reg_msg << "S" << (i + 1) << "=" << switch_state;
            }
            reg_msg << " (0=未触发, 1=触发)";
        } else if (addr == 51) {
            reg_msg << "第一路湿度传感器电压: " << value << " (单位: 1mV, 正常约3300, 越低代表进水越严重)";
        } else if (addr == 52) {
            reg_msg << "第一路温度传感器数值: " << value << " (单位: 0.1°)";
        } else if (addr == 53) {
            reg_msg << "第二路温度传感器数值: " << value << " (单位: 0.1°)";
        } else if (addr == 54) {
            reg_msg << "板载温度传感器数值: " << value << " (单位: 0.1°)";
        } else if (addr == 55) {
            reg_msg << "板载电压传感器数值: " << value << " (单位: 1mV)";
        } else if (addr >= 56 && addr <= 60) {
            // 阀状态寄存器（地址56-60）
            int valve_group_start = (addr - 56) * 8 + 1;
            reg_msg << "阀" << valve_group_start << "-" << (valve_group_start + 7) << "状态: 0x" 
                << std::hex << std::setfill('0') << std::setw(4) << value << std::dec
                << " (每2bit表示1个阀: 0=正常, 2=报警, 3=离线)";
        } else if (addr == 61) {
            reg_msg << "传感器1-16状态: 0x" << std::hex << std::setfill('0') << std::setw(4) << value << std::dec
                << " (每1bit表示1个传感器: 0=正常, 1=报警)";
        } else if (addr == 62) {
            reg_msg << "传感器17-24状态: 0x" << std::hex << std::setfill('0') << std::setw(4) << value << std::dec
                << " (每1bit表示1个传感器: 0=正常, 1=报警)";
        } else if (addr == 63) {
            reg_msg << "主机心跳设置值: " << value << " (心跳值每次发送不同的心跳值)";
        } else if (addr >= 64 && addr <= 103) {
            // 阀1-40电流值设定（地址64-103）
            int valve_num = addr - 64 + 1;
            int16_t current_set = *reinterpret_cast<const int16_t*>(&value);
            reg_msg << "阀" << valve_num << "电流值设定: " << current_set 
                << " (16位有符号, 0~30000对应0mA~1500mA, <0时电流调节至0)";
        } else if (addr == 104) {
            reg_msg << "24V传感器1-16电源开关: 0x" << std::hex << std::setfill('0') << std::setw(4) << value << std::dec
                << " (每1bit表示1个传感器电源: 0=关, 1=开)";
        } else if (addr == 105) {
            reg_msg << "24V传感器17-32电源开关: 0x" << std::hex << std::setfill('0') << std::setw(4) << value << std::dec
                << " (每1bit表示1个传感器电源: 0=关, 1=开)";
        } else if (addr >= 106 && addr <= 108) {
            // 传感器配置（地址106-108）
            int sensor_group_start = (addr - 106) * 8 + 1;
            reg_msg << "传感器" << sensor_group_start << "-" << (sensor_group_start + 7) << "配置: 0x"
                << std::hex << std::setfill('0') << std::setw(4) << value << std::dec
                << " (每2bit表示1个传感器: 00=电压型, 01=电流型)";
        } else if (addr >= 109 && addr <= 128) {
            // 阀频率设定（地址109-128）
            int valve_pair_base = (addr - 109) * 2 + 1;
            uint8_t freq1 = value & 0xFF;
            uint8_t freq2 = (value >> 8) & 0xFF;
            reg_msg << "阀" << valve_pair_base << "-" << (valve_pair_base + 1) << "频率设定: "
                << "V" << valve_pair_base << "=" << static_cast<int>(freq1) << "Hz, "
                << "V" << (valve_pair_base + 1) << "=" << static_cast<int>(freq2) << "Hz "
                << "(输入11~250, 实际输出110~2500Hz)";
        } else if (addr == 129) {
            uint8_t enc1_clear = value & 0xFF;
            uint8_t enc2_clear = (value >> 8) & 0xFF;
            reg_msg << "编码器1-2清0: 编码器1=" << static_cast<int>(enc1_clear) 
                << ", 编码器2=" << static_cast<int>(enc2_clear) << " (写入1要求清除)";
        } else if (addr == 130) {
            reg_msg << "Modbus地址: " << value;
        } else if (addr == 131) {
            std::string baud_str;
            switch (value) {
                case 1: baud_str = "9600bps"; break;
                case 2: baud_str = "19200bps(默认)"; break;
                case 3: baud_str = "38400bps"; break;
                case 4: baud_str = "57600bps"; break;
                case 5: baud_str = "115200bps"; break;
                default: baud_str = "未知"; break;
            }
            reg_msg << "波特率设定选择: " << value << " (" << baud_str << ")";
        } else if (addr == 132) {
            reg_msg << "复位: 0x" << std::hex << std::setfill('0') << std::setw(4) << value << std::dec
                << " (01=RS485, 02=RS422, 0x10=复位)";
        } else {
            reg_msg << "未知寄存器: " << value << " (0x" << std::hex << std::setfill('0') << std::setw(4) << value << std::dec << ")";
        }
        
        DEBUG_VALVE_CONTROL_STATUS_REGISTERS_LOG(reg_msg.str());
    }
}

#ifdef ENABLE_ROS2
void ValveControlProcessor::setROS2Node(rclcpp::Node::SharedPtr ros2_node) {
    ros2_node_ = ros2_node;
    
    if (ros2_node_) {
        // 订阅摇杆话题
        joy_sub_ = ros2_node_->create_subscription<sensor_msgs::msg::Joy>(
            ROS2_TOPIC_JOY, 10,
            std::bind(&ValveControlProcessor::joyCallback, this, std::placeholders::_1));
        DEBUG_VALVE_CONTROL_LOG("Subscribed to ROS2 joystick topic: " << ROS2_TOPIC_JOY);
    }
}

void ValveControlProcessor::joyCallback(const sensor_msgs::msg::Joy::SharedPtr msg) {
    if (!msg) return;
    
    // 检查连接状态，如果断开则不处理摇杆输入
    if (!isConnected()) {
        return;
    }
    
    // 打印接收到的摇杆消息（单行输出，只打印axes）
    std::ostringstream axes_oss;
    axes_oss << "Received joystick axes: [";
    for (size_t i = 0; i < msg->axes.size() && i < static_cast<size_t>(VALVE_COUNT); ++i) {
        if (i > 0) axes_oss << ", ";
        axes_oss << std::fixed << std::setprecision(3) << msg->axes[i];
    }
    axes_oss << "]";
    DEBUG_VALVE_CONTROL_JOYSTICK_LOG(axes_oss.str());
    
    // 更新摇杆轴值
    {
        std::lock_guard<std::mutex> lock(joystick_mutex_);
        last_joystick_axes_ = msg->axes;
    }
    
    // 将摇杆值映射到阀电流（前N个摇杆轴对应前N个阀）
    updateFromJoystickAxes(msg->axes);
}

void ValveControlProcessor::updateFromJoystickAxes(const std::vector<float>& axes) {
    // 检查连接状态，如果断开则不处理摇杆输入
    if (!isConnected()) {
        return;
    }
    
    // 将摇杆轴值映射到阀电流
    // 16个axes映射到32个通道，每个axis拆分成两个正值通道（正向和反向）
    // axis[0] -> valve[0] (正向), valve[1] (反向)
    // axis[1] -> valve[2] (正向), valve[3] (反向)
    // ...
    int max_axes = std::min(static_cast<int>(axes.size()), 16);  // 最多16个axes
    
    for (int i = 0; i < max_axes; ++i) {
        double joystick_value = static_cast<double>(axes[i]);
        
        // 计算两个通道的索引
        int forward_valve = i * 2;      // 正向通道（偶数索引）
        int reverse_valve = i * 2 + 1;  // 反向通道（奇数索引）
        
        // 将axis值拆分成两个正值通道
        // 如果joystick_value > 0，正向通道 = joystick_value，反向通道 = 0
        // 如果joystick_value < 0，正向通道 = 0，反向通道 = -joystick_value（绝对值）
        // 如果joystick_value = 0，两个通道都是0
        double forward_value = (joystick_value > 0) ? joystick_value : 0.0;
        double reverse_value = (joystick_value < 0) ? -joystick_value : 0.0;
        
        // 设置正向通道
        if (forward_valve < VALVE_COUNT) {
            setValveCurrent(forward_valve, forward_value);
        }
        
        // 设置反向通道
        if (reverse_valve < VALVE_COUNT) {
            setValveCurrent(reverse_valve, reverse_value);
        }
    }
}
#endif

