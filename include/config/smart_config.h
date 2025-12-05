/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-01-27 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-01-27 11:29:37
 * @FilePath: include/core/smart_config.h
 * @Description: 智能配置管理器头文件
 * 集成智能串口检测功能的配置管理器
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#ifndef SMART_CONFIG_H
#define SMART_CONFIG_H

#include "mapping.h"
#include "detection/smart_serial_detector.h"
#include <string>
#include <memory>
#include <functional>

/**
 * @brief 智能配置管理器类
 * 
 * 该类负责管理配置和智能串口检测，支持：
 * - 从配置文件加载基础配置
 * - 智能检测和选择最佳串口设备
 * - 动态更新串口配置
 * - 支持多种厂商芯片的自动适配
 */
class SmartConfigManager {
public:
    using SerialPortCallback = std::function<void(const std::string& old_port, const std::string& new_port)>;

    /**
     * @brief 构造函数
     */
    SmartConfigManager();

    /**
     * @brief 析构函数
     */
    ~SmartConfigManager();

    /**
     * @brief 从配置文件加载配置
     * @param config_path 配置文件路径
     * @param enable_smart_detection 是否启用智能串口检测
     * @return 加载成功返回true，失败返回false
     */
    bool loadConfig(const std::string& config_path, bool enable_smart_detection = true);

    /**
     * @brief 获取当前配置
     * @return 配置对象引用
     */
    const Config& getConfig() const;

    /**
     * @brief 获取当前串口路径
     * @return 当前串口路径
     */
    std::string getCurrentSerialPort() const;

    /**
     * @brief 设置串口路径
     * @param port 新的串口路径
     * @return 设置成功返回true，失败返回false
     */
    bool setSerialPort(const std::string& port);

    /**
     * @brief 启用智能串口检测
     * @param enable 是否启用
     */
    void enableSmartDetection(bool enable);

    /**
     * @brief 检查是否启用了智能检测
     * @return 启用返回true，否则返回false
     */
    bool isSmartDetectionEnabled() const;

    /**
     * @brief 手动触发串口检测
     * @return 检测到的设备数量
     */
    int detectSerialDevices();

    /**
     * @brief 获取检测到的串口设备列表
     * @return 设备信息列表
     */
    std::vector<SmartSerialDeviceInfo> getDetectedDevices() const;

    /**
     * @brief 选择最佳串口设备
     * @param preferred_device 首选设备路径（可选）
     * @return 最佳设备路径，未找到返回空字符串
     */
    std::string selectBestDevice(const std::string& preferred_device = "") const;

    /**
     * @brief 设置串口变化回调函数
     * @param callback 回调函数
     */
    void setSerialPortCallback(SerialPortCallback callback);

    /**
     * @brief 添加设备匹配规则
     * @param rule 匹配规则
     */
    void addDeviceRule(const SerialDeviceRule& rule);

    /**
     * @brief 移除设备匹配规则
     * @param rule_name 规则名称
     */
    void removeDeviceRule(const std::string& rule_name);

    /**
     * @brief 获取设备匹配规则列表
     * @return 规则列表
     */
    std::vector<SerialDeviceRule> getDeviceRules() const;

    /**
     * @brief 检查当前串口是否可用
     * @return 可用返回true，否则返回false
     */
    bool isCurrentPortAvailable() const;

    /**
     * @brief 尝试自动切换到可用串口
     * @return 切换成功返回true，失败返回false
     */
    bool autoSwitchToAvailablePort();

    /**
     * @brief 获取串口状态信息
     * @return 状态信息字符串
     */
    std::string getSerialPortStatus() const;

    /**
     * @brief 添加需要排除的串口（用于避免与其他设备冲突）
     * @param port 需要排除的串口路径
     */
    void addExcludedPort(const std::string& port);

    /**
     * @brief 移除排除的串口
     * @param port 需要移除的串口路径
     */
    void removeExcludedPort(const std::string& port);

    /**
     * @brief 清除所有排除的串口
     */
    void clearExcludedPorts();

    /**
     * @brief 手动设置配置（不使用文件加载）
     * @param config 配置对象
     * @return 设置成功返回true，失败返回false
     */
    bool setConfig(const Config& config);

    /**
     * @brief 重置为默认配置
     */
    void resetToDefault();

private:
    std::unique_ptr<Config> config_;                    // 配置对象
    std::unique_ptr<SmartSerialDetector> detector_;     // 智能串口检测器
    bool smart_detection_enabled_;                      // 智能检测是否启用
    std::string original_serial_port_;                   // 原始串口配置
    SerialPortCallback port_callback_;                   // 串口变化回调
    std::vector<std::string> excluded_ports_;           // 需要排除的串口列表（避免与其他设备冲突）

    /**
     * @brief 初始化智能检测器
     * @return 初始化成功返回true，失败返回false
     */
    bool initializeDetector();

    /**
     * @brief 处理串口设备连接事件
     * @param device_path 设备路径
     * @param connected 连接状态
     */
    void onDeviceConnectionChange(const std::string& device_path, bool connected);

    /**
     * @brief 更新串口配置
     * @param new_port 新的串口路径
     */
    void updateSerialPort(const std::string& new_port);

    /**
     * @brief 解析串口路径（符号链接到实际设备）
     * @param path 设备路径（可能是符号链接）
     * @return 实际设备路径
     */
    std::string resolvePortPath(const std::string& path) const;
};

#endif // SMART_CONFIG_H
