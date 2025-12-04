/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:29:37
 * @FilePath: include/joystick_detector.h
 * @Description: USB物理手柄检测器头文件
 * 用于检测和管理系统中连接的USB物理手柄设备
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

/**
 * @brief USB物理手柄设备信息结构
 */
struct PhysicalJoystickInfo {
    std::string device_path;      ///< 设备路径，如/dev/input/js0
    std::string device_name;      ///< 设备名称
    std::string vendor_name;      ///< 厂商名称
    std::string product_name;     ///< 产品名称
    uint16_t vendor_id;           ///< 厂商ID
    uint16_t product_id;          ///< 产品ID
    uint16_t version;             ///< 版本号
    int num_axes;                 ///< 轴数量
    int num_buttons;              ///< 按钮数量
    bool is_gamepad;              ///< 是否为游戏手柄
    bool is_joystick;             ///< 是否为摇杆
    bool is_connected;            ///< 是否已连接
};

/**
 * @brief 手柄事件回调函数类型
 */
using JoystickEventCallback = std::function<void(const std::string& device_path, 
                                                 int event_type, 
                                                 int code, 
                                                 int value)>;

/**
 * @brief USB物理手柄检测器类
 * 
 * 该类负责：
 * - 自动检测系统中连接的USB物理手柄
 * - 监控手柄的连接/断开状态
 * - 读取手柄输入事件
 * - 提供手柄信息查询接口
 * - 支持热插拔检测
 */
class JoystickDetector {
public:
    /**
     * @brief 事件类型枚举
     */
    enum EventType {
        EVENT_AXIS = 0,           ///< 轴事件
        EVENT_BUTTON = 1,         ///< 按钮事件
        EVENT_CONNECT = 2,        ///< 连接事件
        EVENT_DISCONNECT = 3      ///< 断开事件
    };

    /**
     * @brief 默认构造函数
     */
    JoystickDetector();

    /**
     * @brief 析构函数
     */
    ~JoystickDetector();

    /**
     * @brief 启动检测器
     * @param enable_hotplug 是否启用热插拔检测
     * @return 启动成功返回true
     */
    bool start(bool enable_hotplug = true);

    /**
     * @brief 停止检测器
     */
    void stop();

    /**
     * @brief 扫描当前连接的手柄
     * @return 扫描到的手柄数量
     */
    int scanDevices();

    /**
     * @brief 获取所有已连接手柄的信息
     * @return 手柄信息列表
     */
    std::vector<PhysicalJoystickInfo> getConnectedDevices() const;

    /**
     * @brief 根据设备路径获取手柄信息
     * @param device_path 设备路径
     * @return 手柄信息，如果未找到返回nullptr
     */
    std::shared_ptr<PhysicalJoystickInfo> getDeviceInfo(const std::string& device_path) const;

    /**
     * @brief 设置事件回调函数
     * @param callback 回调函数
     */
    void setEventCallback(JoystickEventCallback callback);

    /**
     * @brief 检查指定设备是否已连接
     * @param device_path 设备路径
     * @return 已连接返回true
     */
    bool isDeviceConnected(const std::string& device_path) const;

    /**
     * @brief 获取检测器状态
     * @return 正在运行返回true
     */
    bool isRunning() const { return running_.load(); }

    /**
     * @brief 设置扫描间隔（毫秒）
     * @param interval_ms 扫描间隔
     */
    void setScanInterval(int interval_ms);

    /**
     * @brief 获取扫描间隔
     * @return 扫描间隔（毫秒）
     */
    int getScanInterval() const { return scan_interval_ms_; }

private:
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
     * @return 设备信息
     */
    std::shared_ptr<PhysicalJoystickInfo> readDeviceInfo(const std::string& device_path);

    /**
     * @brief 检查设备是否为手柄
     * @param device_path 设备路径
     * @return 是手柄返回true
     */
    bool isJoystickDevice(const std::string& device_path);

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
     * @brief 读取设备事件
     * @param device_path 设备路径
     */
    void readDeviceEvents(const std::string& device_path);

    /**
     * @brief 解析设备名称字符串
     * @param name_str 名称字符串
     * @return 解析后的名称
     */
    std::string parseDeviceName(const std::string& name_str);

    std::atomic<bool> running_{false};                    ///< 运行状态
    std::atomic<bool> enable_hotplug_{true};              ///< 热插拔开关
    std::thread scan_thread_;                             ///< 扫描线程
    std::thread hotplug_thread_;                          ///< 热插拔监控线程
    mutable std::mutex devices_mutex_;                            ///< 设备列表互斥锁
    std::map<std::string, std::shared_ptr<PhysicalJoystickInfo>> devices_; ///< 设备映射表
    JoystickEventCallback event_callback_;                ///< 事件回调函数
    int scan_interval_ms_{1000};                          ///< 扫描间隔（毫秒）
    std::vector<std::thread> event_threads_;              ///< 事件读取线程列表
    std::atomic<bool> stop_event_threads_{false};         ///< 停止事件线程标志
};
