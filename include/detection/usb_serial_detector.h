/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-01-27 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-01-27 11:29:37
 * @FilePath: include/core/usb_serial_detector.h
 * @Description: USB转串口设备检测器头文件
 * 实现USB转串口设备的自动检测、热插拔监控和自动恢复功能
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#ifndef USB_SERIAL_DETECTOR_H
#define USB_SERIAL_DETECTOR_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <chrono>
#include "constants.h"

/**
 * @brief USB转串口设备信息结构
 */
struct USBSerialDeviceInfo {
    std::string device_path;           // 设备路径，如 /dev/ttyUSB0
    std::string device_name;           // 设备名称
    std::string vendor_id;             // 厂商ID
    std::string product_id;            // 产品ID
    std::string serial_number;         // 序列号
    std::string driver_name;           // 驱动名称
    std::string usb_path;              // USB路径
    bool is_connected;                 // 连接状态
    std::chrono::steady_clock::time_point last_seen; // 最后检测时间
    int reconnect_attempts;            // 重连尝试次数
    std::chrono::steady_clock::time_point last_reconnect_attempt; // 最后重连尝试时间
};

/**
 * @brief USB转串口设备检测器类
 * 
 * 该类负责检测和管理USB转串口设备，支持：
 * - 自动检测系统中的USB转串口设备
 * - 热插拔监控
 * - 设备状态跟踪
 * - 自动重连机制
 */
class USBSerialDetector {
public:
    using DeviceCallback = std::function<void(const std::string& device_path, bool connected)>;
    using DeviceList = std::map<std::string, std::shared_ptr<USBSerialDeviceInfo>>;

    /**
     * @brief 构造函数
     */
    USBSerialDetector();

    /**
     * @brief 析构函数
     */
    ~USBSerialDetector();

    /**
     * @brief 启动设备检测器
     * @param enable_hotplug 是否启用热插拔监控
     * @return 启动成功返回true，失败返回false
     */
    bool start(bool enable_hotplug = true);

    /**
     * @brief 停止设备检测器
     */
    void stop();

    /**
     * @brief 扫描USB转串口设备
     * @return 找到的设备数量
     */
    int scanDevices();

    /**
     * @brief 获取已连接的设备列表
     * @return 设备信息列表
     */
    std::vector<USBSerialDeviceInfo> getConnectedDevices() const;

    /**
     * @brief 获取设备数量
     * @return 已连接设备数量
     */
    int getConnectedDeviceCount() const;

    /**
     * @brief 检查指定设备是否连接
     * @param device_path 设备路径
     * @return 设备连接返回true，否则返回false
     */
    bool isDeviceConnected(const std::string& device_path) const;

    /**
     * @brief 获取设备信息
     * @param device_path 设备路径
     * @return 设备信息指针，未找到返回nullptr
     */
    std::shared_ptr<USBSerialDeviceInfo> getDeviceInfo(const std::string& device_path) const;

    /**
     * @brief 设置设备状态变化回调函数
     * @param callback 回调函数
     */
    void setDeviceCallback(DeviceCallback callback);

    /**
     * @brief 手动触发设备重连
     * @param device_path 设备路径
     * @return 重连成功返回true，失败返回false
     */
    bool reconnectDevice(const std::string& device_path);

    /**
     * @brief 获取所有可能的串口设备路径
     * @return 设备路径列表
     */
    std::vector<std::string> getAllSerialDevices() const;

    /**
     * @brief 根据设备特征查找设备
     * @param vendor_id 厂商ID（可选）
     * @param product_id 产品ID（可选）
     * @param serial_number 序列号（可选）
     * @return 匹配的设备路径列表
     */
    std::vector<std::string> findDevicesByCharacteristics(const std::string& vendor_id = "",
                                                         const std::string& product_id = "",
                                                         const std::string& serial_number = "") const;

private:
    std::atomic<bool> running_;                    // 运行状态
    std::atomic<bool> enable_hotplug_;            // 热插拔监控状态
    std::atomic<bool> stop_event_threads_;        // 停止事件线程标志
    
    mutable std::mutex devices_mutex_;            // 设备列表互斥锁
    DeviceList devices_;                          // 设备列表
    
    std::thread scan_thread_;                     // 扫描线程
    std::thread hotplug_thread_;                  // 热插拔监控线程
    std::vector<std::thread> event_threads_;      // 事件处理线程列表
    
    DeviceCallback device_callback_;              // 设备状态变化回调
    
    // 扫描间隔和重连参数（使用constants.h中的宏定义）
    static constexpr int SCAN_INTERVAL_MS = USB_SERIAL_SCAN_INTERVAL_MS;        // 扫描间隔（毫秒）
    static constexpr int RECONNECT_INTERVAL_MS = USB_SERIAL_RECONNECT_INTERVAL_MS;    // 重连间隔（毫秒）
    static constexpr int MAX_RECONNECT_ATTEMPTS = USB_SERIAL_MAX_RECONNECT_ATTEMPTS;     // 最大重连尝试次数

    /**
     * @brief 扫描线程主函数
     */
    void scanThread();

    /**
     * @brief 热插拔监控线程主函数
     */
    void hotplugThread();

    /**
     * @brief 读取设备信息
     * @param device_path 设备路径
     * @return 设备信息指针
     */
    std::shared_ptr<USBSerialDeviceInfo> readDeviceInfo(const std::string& device_path);

    /**
     * @brief 检查是否为USB转串口设备
     * @param device_path 设备路径
     * @return 是USB转串口设备返回true，否则返回false
     */
    bool isUSBSerialDevice(const std::string& device_path);

    /**
     * @brief 处理设备连接
     * @param device_path 设备路径
     */
    void handleDeviceConnect(const std::string& device_path);

    /**
     * @brief 处理设备断开
     * @param device_path 设备路径
     */
    void handleDeviceDisconnect(const std::string& device_path);

    /**
     * @brief 从sysfs读取USB设备信息
     * @param device_path 设备路径
     * @param info 设备信息结构
     */
    void readUSBInfoFromSysfs(const std::string& device_path, USBSerialDeviceInfo& info);

    /**
     * @brief 解析设备名称
     * @param raw_name 原始设备名称
     * @return 解析后的设备名称
     */
    std::string parseDeviceName(const std::string& raw_name);

    /**
     * @brief 检查设备是否仍然存在
     * @param device_path 设备路径
     * @return 设备存在返回true，否则返回false
     */
    bool checkDeviceExists(const std::string& device_path);

    /**
     * @brief 自动重连断开的设备
     */
    void autoReconnectDevices();
};

#endif // USB_SERIAL_DETECTOR_H
