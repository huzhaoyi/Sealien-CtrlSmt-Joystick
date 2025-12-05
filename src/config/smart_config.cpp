/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-10-23 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-10-23 11:29:37
 * @FilePath: src/core/smart_config.cpp
 * @Description: 智能配置管理器实现文件
 * 集成智能串口检测功能的配置管理器
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "smart_config.h"
#include "utils.h"
#include "constants.h"
#include <algorithm>
#include <limits.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <vector>

SmartConfigManager::SmartConfigManager() 
    : smart_detection_enabled_(false) {
}

SmartConfigManager::~SmartConfigManager() {
    if (detector_) {
        detector_->stop();
    }
}

bool SmartConfigManager::loadConfig(const std::string& config_path, bool enable_smart_detection) {
    try {
        // 加载基础配置
        config_ = std::make_unique<Config>(ConfigLoader::Load(config_path));
        original_serial_port_ = config_->getSerialPort();
        
        DEBUG_CORE_LOG("配置已成功从以下路径加载: " << config_path);
        DEBUG_CORE_LOG("原始串口: " << original_serial_port_);
        
        // 启用智能检测
        smart_detection_enabled_ = enable_smart_detection;
        if (enable_smart_detection) {
            if (!initializeDetector()) {
                DEBUG_CORE_WARNING("智能串口检测器初始化失败，使用静态配置");
                smart_detection_enabled_ = false;
            } else {
                DEBUG_CORE_LOG("智能串口检测已启用");
                
                // 执行初始检测
                int device_count = detectSerialDevices();
                DEBUG_CORE_LOG("检测到 " << device_count << " 个串口设备");
                
                // 如果配置的是符号链接，先解析符号链接找到实际设备
                std::string preferred_device = original_serial_port_;
                if (original_serial_port_.find("/dev/serial/by-id/") == 0 || 
                    original_serial_port_.find("/dev/serial/by-path/") == 0) {
                    // 是符号链接，尝试解析
                    char resolved_path[PATH_MAX];
                    if (realpath(original_serial_port_.c_str(), resolved_path) != nullptr) {
                        preferred_device = std::string(resolved_path);
                        DEBUG_CORE_LOG("解析符号链接: " << original_serial_port_ << " -> " << preferred_device);
                    }
                }
                
                // 尝试选择最佳设备，优先使用配置的设备（或解析后的设备）
                std::string best_device = selectBestDevice(preferred_device);
                
                // 如果首选设备存在且可用，优先使用它
                if (!best_device.empty()) {
                    if (best_device == preferred_device || best_device == original_serial_port_) {
                        // 使用首选设备
                        if (best_device != config_->getSerialPort()) {
                            DEBUG_CORE_LOG("使用首选设备: " << best_device << " (来自配置: " << original_serial_port_ << ")");
                            updateSerialPort(best_device);
                        }
                    } else {
                        // 首选设备不可用，使用智能检测选择的设备
                        DEBUG_CORE_LOG("自动选择最佳设备: " << best_device << " (首选: " << preferred_device << ", 原始: " << original_serial_port_ << ")");
                    updateSerialPort(best_device);
                    }
                } else {
                    DEBUG_CORE_WARNING("未找到合适的串口设备，使用原始配置");
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        DEBUG_CORE_ERROR("无法加载配置: " << e.what());
        return false;
    }
}

const Config& SmartConfigManager::getConfig() const {
    return *config_;
}

std::string SmartConfigManager::getCurrentSerialPort() const {
    return config_->getSerialPort();
}

bool SmartConfigManager::setSerialPort(const std::string& port) {
    if (port.empty()) {
        DEBUG_CORE_ERROR("无法设置空串口");
        return false;
    }
    
    // 检查新端口是否可用
    if (detector_ && !detector_->isDeviceConnected(port)) {
        DEBUG_CORE_WARNING("串口 " << port << " 当前不可用");
        // 仍然允许设置，因为设备可能稍后连接
    }
    
    updateSerialPort(port);
    return true;
}

void SmartConfigManager::enableSmartDetection(bool enable) {
    if (enable == smart_detection_enabled_) {
        return; // 状态未改变
    }
    
    smart_detection_enabled_ = enable;
    
    if (enable) {
        if (!detector_) {
            initializeDetector();
        }
        if (detector_) {
            DEBUG_CORE_LOG("智能串口检测已启用");
            detectSerialDevices();
        }
    } else {
        if (detector_) {
            detector_->stop();
            detector_.reset();
        }
        DEBUG_CORE_LOG("智能串口检测已禁用");
    }
}

bool SmartConfigManager::isSmartDetectionEnabled() const {
    return smart_detection_enabled_ && detector_ != nullptr;
}

int SmartConfigManager::detectSerialDevices() {
    if (!detector_) {
        return 0;
    }
    
    return detector_->scanDevices();
}

std::vector<SmartSerialDeviceInfo> SmartConfigManager::getDetectedDevices() const {
    if (!detector_) {
        return {};
    }
    
    return detector_->getConnectedDevices();
}

std::string SmartConfigManager::selectBestDevice(const std::string& preferred_device) const {
    if (!detector_) {
        return preferred_device;
    }
    
    // 获取所有检测到的设备
    auto devices = detector_->getConnectedDevices();
    
    // 如果指定了首选设备且该设备存在且未被排除，优先选择
    if (!preferred_device.empty()) {
        bool is_excluded = false;
        for (const auto& excluded : excluded_ports_) {
            if (preferred_device == excluded || resolvePortPath(preferred_device) == resolvePortPath(excluded)) {
                is_excluded = true;
                break;
            }
        }
        
        if (!is_excluded) {
            for (const auto& device : devices) {
                if (device.device_path == preferred_device || 
                    resolvePortPath(device.device_path) == resolvePortPath(preferred_device)) {
                    DEBUG_CORE_LOG("已选择首选设备: " << preferred_device);
                    return preferred_device;
                }
            }
        }
    }
    
    // 根据优先级选择最佳设备，排除已占用的串口
    std::string best_device;
    int best_priority = -1;
    
    for (const auto& device : devices) {
        // 检查是否被排除
        bool is_excluded = false;
        for (const auto& excluded : excluded_ports_) {
            if (device.device_path == excluded || 
                resolvePortPath(device.device_path) == resolvePortPath(excluded)) {
                is_excluded = true;
                break;
            }
        }
        
        if (is_excluded) {
            continue; // 跳过被排除的设备
        }
        
        if (device.priority > best_priority) {
            best_priority = device.priority;
            best_device = device.device_path;
        }
    }
    
    if (!best_device.empty()) {
        DEBUG_CORE_LOG("已选择最佳设备: " << best_device << " (优先级: " << best_priority << ")");
    }
    
    return best_device;
}

std::string SmartConfigManager::resolvePortPath(const std::string& path) const {
    // 解析符号链接到实际设备路径
    char resolved_path[PATH_MAX];
    if (realpath(path.c_str(), resolved_path) != nullptr) {
        return std::string(resolved_path);
    }
    return path; // 如果解析失败，返回原路径
}

void SmartConfigManager::setSerialPortCallback(SerialPortCallback callback) {
    port_callback_ = callback;
}

void SmartConfigManager::addDeviceRule(const SerialDeviceRule& rule) {
    if (detector_) {
        detector_->addDeviceRule(rule);
    }
}

void SmartConfigManager::removeDeviceRule(const std::string& rule_name) {
    if (detector_) {
        detector_->removeDeviceRule(rule_name);
    }
}

std::vector<SerialDeviceRule> SmartConfigManager::getDeviceRules() const {
    if (!detector_) {
        return {};
    }
    
    return detector_->getDeviceRules();
}

bool SmartConfigManager::isCurrentPortAvailable() const {
    if (!detector_) {
        // 如果没有检测器，假设端口可用
        return true;
    }
    
    return detector_->isDeviceConnected(config_->getSerialPort());
}

bool SmartConfigManager::autoSwitchToAvailablePort() {
    if (!detector_) {
        return false;
    }
    
    // 检测设备
    detectSerialDevices();
    
    // 选择最佳设备（排除已占用的串口）
    std::string best_device = selectBestDevice();
    if (!best_device.empty() && best_device != config_->getSerialPort()) {
        DEBUG_CORE_LOG("自动切换到可用端口: " << best_device);
        updateSerialPort(best_device);
        return true;
    }
    
    return false;
}

void SmartConfigManager::addExcludedPort(const std::string& port) {
    if (port.empty()) {
        return;
    }
    
    // 检查是否已存在
    for (const auto& excluded : excluded_ports_) {
        if (excluded == port) {
            return; // 已存在，无需重复添加
        }
    }
    
    excluded_ports_.push_back(port);
    DEBUG_CORE_LOG("已添加排除端口: " << port);
}

void SmartConfigManager::removeExcludedPort(const std::string& port) {
    excluded_ports_.erase(
        std::remove(excluded_ports_.begin(), excluded_ports_.end(), port),
        excluded_ports_.end()
    );
    DEBUG_CORE_LOG("已移除排除端口: " << port);
}

void SmartConfigManager::clearExcludedPorts() {
    excluded_ports_.clear();
    DEBUG_CORE_LOG("已清除所有排除端口");
}

std::string SmartConfigManager::getSerialPortStatus() const {
    std::ostringstream oss;
    oss << "当前端口: " << config_->getSerialPort();
    oss << ", Smart detection: " << (smart_detection_enabled_ ? "enabled" : "disabled");
    
    if (detector_) {
        oss << ", Detected devices: " << detector_->getConnectedDeviceCount();
        oss << ", Current port available: " << (isCurrentPortAvailable() ? "yes" : "no");
        
        auto devices = detector_->getConnectedDevices();
        if (!devices.empty()) {
            oss << ", Available devices: [";
            for (size_t i = 0; i < devices.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << devices[i].device_path << "(" << devices[i].chip_type << ")";
            }
            oss << "]";
        }
    }
    
    return oss.str();
}

bool SmartConfigManager::setConfig(const Config& config) {
    try {
        config_ = std::make_unique<Config>(config);
        original_serial_port_ = config.getSerialPort();
        
        DEBUG_CORE_LOG("配置已手动设置");
        DEBUG_CORE_LOG("串口: " << config.getSerialPort());
        
        return true;
    } catch (const std::exception& e) {
        DEBUG_CORE_ERROR("无法设置配置: " << e.what());
        return false;
    }
}

void SmartConfigManager::resetToDefault() {
    if (!original_serial_port_.empty()) {
        updateSerialPort(original_serial_port_);
        DEBUG_CORE_LOG("已重置为原始串口: " << original_serial_port_);
    }
}

bool SmartConfigManager::initializeDetector() {
    try {
        detector_ = std::make_unique<SmartSerialDetector>();
        
        // 设置USB厂商配置文件路径
        std::string vendor_config_path = "config/usb/usb_vendors.yaml";
        detector_->setVendorConfigPath(vendor_config_path);
        
        // 设置设备连接回调
        detector_->setDeviceCallback([this](const std::string& device_path, bool connected) {
            onDeviceConnectionChange(device_path, connected);
        });
        
        // 启动检测器
        if (!detector_->start(true)) {
            DEBUG_CORE_ERROR("无法启动智能串口检测器");
            detector_.reset();
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        DEBUG_CORE_ERROR("无法初始化智能串口检测器: " << e.what());
        detector_.reset();
        return false;
    }
}

void SmartConfigManager::onDeviceConnectionChange(const std::string& device_path, bool connected) {
    DEBUG_CORE_LOG("设备 " << device_path << " " << (connected ? "已连接" : "已断开"));
    
    // 简化回调逻辑，避免在回调中进行复杂的操作
    if (connected) {
        DEBUG_CORE_LOG("设备已连接: " << device_path);
        // 延迟处理设备连接，避免在回调中阻塞
        std::thread([this, device_path]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // 如果当前没有可用端口，尝试选择新连接的设备
            if (smart_detection_enabled_ && !isCurrentPortAvailable()) {
                DEBUG_CORE_LOG("当前无可用端口，检查新设备是否合适");
                
                std::string best_device = selectBestDevice();
                if (!best_device.empty() && best_device != config_->getSerialPort()) {
                    DEBUG_CORE_LOG("自动选择新设备: " << best_device);
                    updateSerialPort(best_device);
                }
            }
        }).detach();
    } else {
        DEBUG_CORE_LOG("设备已断开: " << device_path);
        // 如果当前使用的设备断开连接，尝试切换到其他可用设备
        if (device_path == config_->getSerialPort()) {
            DEBUG_CORE_WARNING("当前串口 " << device_path << " 已断开，正在尝试自动切换");
            
            // 延迟处理设备断开，避免在回调中阻塞
            std::thread([this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                
                if (!autoSwitchToAvailablePort()) {
                    DEBUG_CORE_ERROR("无法自动切换到可用端口，当前端口不可用");
                }
            }).detach();
        }
    }
}

void SmartConfigManager::updateSerialPort(const std::string& new_port) {
    if (new_port == config_->getSerialPort()) {
        return; // 端口未改变
    }
    
    std::string old_port = config_->getSerialPort();
    config_->serial_port = new_port;
    
    DEBUG_CORE_LOG("串口已更新: " << old_port << " -> " << new_port);
    
    // 调用回调函数
    if (port_callback_) {
        port_callback_(old_port, new_port);
    }
}
