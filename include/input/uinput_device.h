/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:29:04
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:29:04
 * @FilePath: include/uinput_device.h
 * @Description: Linux uinput虚拟输入设备头文件
 * 定义虚拟输入设备类，用于创建和管理Linux内核中的虚拟摇杆设备
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */
#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include "constants.h"

/**
 * @brief Linux uinput虚拟输入设备类
 * 
 * 该类封装了Linux uinput接口，用于创建虚拟输入设备。
 * 支持创建虚拟摇杆、手柄等输入设备，可以模拟轴和按键输入。
 * 
 * 主要功能：
 * - 创建虚拟输入设备
 * - 配置轴的取值范围和属性
 * - 发送轴和按键事件
 * - 管理设备生命周期
 * 
 * 使用流程：
 * 1. 创建对象
 * 2. 调用create()创建设备
 * 3. 使用setAbsRange()配置轴属性
 * 4. 使用sendAbs()和sendKey()发送事件
 * 5. 调用sync()同步事件
 * 6. 析构时自动销毁设备
 */
class UInputDevice {
public:
    /**
     * @brief 默认构造函数
     */
    UInputDevice() = default;
    
    /**
     * @brief 析构函数
     * 自动销毁虚拟设备并释放资源
     */
    ~UInputDevice();

    /**
     * @brief 创建虚拟输入设备
     * @param name 设备名称，会显示在系统设备列表中
     * @param vendor 厂商ID（16位）
     * @param product 产品ID（16位）
     * @param version 版本号（16位）
     * @param abs_codes 支持的轴代码数组
     * @param key_codes 支持的按键代码数组
     * @return 创建成功返回true，失败返回false
     */
    bool create(const std::string& name,
                uint16_t vendor = VIRTUAL_DEVICE_VENDOR_ID, 
                uint16_t product = VIRTUAL_DEVICE_PRODUCT_ID, 
                uint16_t version = VIRTUAL_DEVICE_VERSION,
                const std::vector<int>& abs_codes = {},
                const std::vector<int>& key_codes = {});
    
    /**
     * @brief 设置轴的取值范围和属性
     * @param code 轴代码（如ABS_X, ABS_Y等）
     * @param minv 最小值
     * @param maxv 最大值
     * @param fuzz 模糊值，用于模拟真实设备的噪声
     * @param flat 平坦值，表示轴在中心位置附近的死区
     * @param resolution 分辨率，表示轴值的精度
     */
    void setAbsRange(int code, int minv, int maxv, int fuzz=0, int flat=0, int resolution=0);
    
    /**
     * @brief 发送轴事件
     * @param code 轴代码
     * @param value 轴值，应在setAbsRange设置的范围内
     */
    void sendAbs(int code, int value);
    
    /**
     * @brief 发送按键事件
     * @param code 按键代码
     * @param value 按键状态：0=释放，1=按下，2=重复
     */
    void sendKey(int code, int value);
    
    /**
     * @brief 同步所有输入事件
     * 
     * 通知Linux内核处理之前发送的所有事件。
     * 在发送完一组相关事件后必须调用此函数。
     */
    void sync();
    
    /**
     * @brief 销毁虚拟设备
     * 
     * 通知内核销毁设备并关闭文件描述符。
     * 析构函数会自动调用此函数。
     */
    void destroy();

    /**
     * @brief 检查设备状态
     * @return 如果设备已创建且有效则返回true
     */
    bool ok() const { return fd_ >= 0; }

private:
    int fd_{-1};    ///< 设备文件描述符，-1表示无效
};
