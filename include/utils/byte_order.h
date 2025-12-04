/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-01-27
 * @Description: 字节序转换工具
 * 处理Modbus数据的字节序转换问题
 */

#ifndef BYTE_ORDER_H
#define BYTE_ORDER_H

#include <cstdint>
#include <arpa/inet.h>

namespace ByteOrder {

/**
 * @brief 检查系统是否为小端序
 * @return true if little-endian, false if big-endian
 */
inline bool isLittleEndian() {
    union {
        uint16_t value;
        uint8_t bytes[2];
    } test = {0x0102};
    return test.bytes[0] == 0x02;
}

/**
 * @brief 16位字节序转换
 * @param value 原始16位值
 * @return 转换后的16位值
 */
inline uint16_t swapBytes16(uint16_t value) {
    return ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
}

/**
 * @brief 32位字节序转换
 * @param value 原始32位值
 * @param value 转换后的32位值
 */
inline uint32_t swapBytes32(uint32_t value) {
    return ((value & 0xFF000000) >> 24) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x000000FF) << 24);
}

/**
 * @brief 将网络字节序（大端序）转换为主机字节序
 * @param value 网络字节序的16位值
 * @return 主机字节序的16位值
 */
inline uint16_t ntohs(uint16_t value) {
    return ::ntohs(value);
}



/**
 * @brief 修正Modbus寄存器值的字节序
 * @param value 原始寄存器值
 * @param swap_endian 是否需要交换字节序
 * @return 修正后的值
 */
inline uint16_t fixModbusValue(uint16_t value, bool swap_endian = true) {
    if (swap_endian) {
        return swapBytes16(value);
    }
    return value;
}

/**
 * @brief 将16位无符号整数转换为有符号整数
 * @param value 16位无符号整数
 * @return 有符号整数
 */
inline int16_t toSigned16(uint16_t value) {
    return static_cast<int16_t>(value);
}

/**
 * @brief 将修正后的Modbus值转换为有符号整数
 * @param value 原始Modbus值
 * @param swap_endian 是否需要交换字节序
 * @return 有符号整数
 */
inline int16_t fixModbusValueSigned(uint16_t value, bool swap_endian = true) {
    uint16_t fixed = fixModbusValue(value, swap_endian);
    return toSigned16(fixed);
}

} // namespace ByteOrder

#endif // BYTE_ORDER_H
