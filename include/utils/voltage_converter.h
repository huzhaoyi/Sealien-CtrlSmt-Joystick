/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-01-27
 * @Description: Modbus数据到电压/电流转换工具
 * 将Modbus寄存器值转换为实际的电压和电流值
 */

#ifndef VOLTAGE_CONVERTER_H
#define VOLTAGE_CONVERTER_H

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

namespace VoltageConverter {

/**
 * @brief 通道类型枚举
 */
enum class ChannelType {
    VOLTAGE_0_10V,    // 0-10V电压输入
    CURRENT_0_20MA    // 0-20mA电流输入
};

/**
 * @brief 转换结果结构
 */
struct ConversionResult {
    double voltage;        // 电压值 (V)
    double current;        // 电流值 (mA)
    double percentage;     // 百分比 (0-100%)
    std::string description; // 描述信息
};

/**
 * @brief 将Modbus寄存器值转换为电压值
 * @param raw_value 原始Modbus寄存器值
 * @param channel_type 通道类型
 * @param modbus_max_value Modbus寄存器最大值（默认4095，对应标准12位ADC）
 * @return 转换结果
 */
inline ConversionResult convertToVoltage(uint16_t raw_value, ChannelType channel_type, uint16_t modbus_max_value = 4095) {
    ConversionResult result;
    
    // 保存原始值用于计算（不限制，允许显示超出范围的值）
    uint16_t original_value = raw_value;
    
    // 使用配置的Modbus最大值范围
    uint16_t max_range = modbus_max_value;
    
    // 计算百分比
    result.percentage = (static_cast<double>(original_value) / static_cast<double>(max_range)) * 100.0;
    // 限制百分比在0-100%范围内
    result.percentage = std::max(0.0, std::min(100.0, result.percentage));
    
    switch (channel_type) {
        case ChannelType::VOLTAGE_0_10V:
            // 使用配置的Modbus最大值范围对应0-10V
            result.voltage = (static_cast<double>(original_value) / static_cast<double>(max_range)) * 10.0;
            // 限制电压在0-10V范围内
            result.voltage = std::max(0.0, std::min(10.0, result.voltage));
            result.current = 0.0; // 电压通道不计算电流
            result.description = "电压输入通道";
            break;
            
        case ChannelType::CURRENT_0_20MA:
            // 电流通道类似处理
            result.current = (static_cast<double>(original_value) / static_cast<double>(max_range)) * 20.0;
            result.current = std::max(0.0, std::min(20.0, result.current));
            result.voltage = 0.0; // 电流通道不计算电压
            result.description = "电流输入通道";
            break;
    }
    
    return result;
}

/**
 * @brief 将电压值转换为Modbus寄存器值
 * @param voltage 电压值 (V)
 * @param channel_type 通道类型
 * @return Modbus寄存器值 (0-4095)
 */
inline uint16_t convertFromVoltage(double voltage, ChannelType channel_type) {
    uint16_t result = 0;
    
    switch (channel_type) {
        case ChannelType::VOLTAGE_0_10V:
            // 0-10V 对应 0-4095
            voltage = std::max(0.0, std::min(10.0, voltage));
            result = static_cast<uint16_t>((voltage / 10.0) * 4095.0);
            break;
            
        case ChannelType::CURRENT_0_20MA:
            // 0-20mA 对应 0-4095
            voltage = std::max(0.0, std::min(20.0, voltage));
            result = static_cast<uint16_t>((voltage / 20.0) * 4095.0);
            break;
    }
    
    return result;
}

/**
 * @brief 格式化转换结果为字符串
 * @param result 转换结果
 * @param channel_type 通道类型
 * @return 格式化的字符串
 */
inline std::string formatResult(const ConversionResult& result, ChannelType channel_type) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    
    switch (channel_type) {
        case ChannelType::VOLTAGE_0_10V:
            oss << "电压: " << result.voltage << "V (" << result.percentage << "%)";
            break;
            
        case ChannelType::CURRENT_0_20MA:
            oss << "电流: " << result.current << "mA (" << result.percentage << "%)";
            break;
    }
    
    return oss.str();
}

/**
 * @brief 获取通道类型描述
 * @param channel_type 通道类型
 * @return 描述字符串
 */
inline std::string getChannelTypeDescription(ChannelType channel_type) {
    switch (channel_type) {
        case ChannelType::VOLTAGE_0_10V:
            return "0-10V电压输入";
        case ChannelType::CURRENT_0_20MA:
            return "0-20mA电流输入";
        default:
            return "未知通道类型";
    }
}

/**
 * @brief 检查Modbus值是否在正常范围内
 * @param raw_value 原始Modbus值
 * @param channel_type 通道类型
 * @return true if 值在正常范围内
 */
inline bool isValueInNormalRange(uint16_t raw_value, ChannelType channel_type) {
    // 对于电压通道，正常范围通常是 0.5V-9.5V (约205-3891)
    // 对于电流通道，正常范围通常是 1mA-19mA (约205-3891)
    const uint16_t min_normal = 205;   // 约5%的量程
    const uint16_t max_normal = 3891; // 约95%的量程
    
    // 避免未使用参数警告
    (void)channel_type;
    
    return raw_value >= min_normal && raw_value <= max_normal;
}

/**
 * @brief 获取通道状态描述
 * @param raw_value 原始Modbus值
 * @param channel_type 通道类型
 * @param modbus_max_value Modbus寄存器最大值（默认4095）
 * @return 状态描述
 */
inline std::string getChannelStatus(uint16_t raw_value, ChannelType channel_type, uint16_t modbus_max_value = 4095) {
    // 避免未使用参数警告
    (void)channel_type;
    
    // 根据配置的最大值动态计算阈值
    uint16_t min_threshold = 10;  // 最小值阈值（固定）
    uint16_t max_threshold = modbus_max_value - 10;  // 最大值阈值（接近最大值时判定为过载）
    
    // 计算正常范围（5%-95%）
    uint16_t min_normal = static_cast<uint16_t>(modbus_max_value * 0.05);
    uint16_t max_normal = static_cast<uint16_t>(modbus_max_value * 0.95);
    
    if (raw_value < min_threshold) {
        return "未连接或短路";
    } else if (raw_value > max_threshold) {
        return "过载或开路";
    } else if (raw_value >= min_normal && raw_value <= max_normal) {
        return "正常";
    } else {
        return "异常";
    }
}

} // namespace VoltageConverter

#endif // VOLTAGE_CONVERTER_H
