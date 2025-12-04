/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-10-23 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-10-23 11:29:37
 * @FilePath: src/core/smart_serial_detector.cpp
 * @Description: 智能串口检测器实现文件
 * 实现USB转串口设备的智能检测和自动适配功能，支持多种厂商芯片
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "smart_serial_detector.h"
#include "config/usb_vendor_config.h"
#include "utils.h"
#include "constants.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <regex>

SmartSerialDetector::SmartSerialDetector() 
    : running_(false), enable_hotplug_(false), stop_event_threads_(false) {
    // 初始化USB厂商配置
    vendor_config_ = std::make_unique<USBVendorConfig>();
    initializeDefaultRules();
}

SmartSerialDetector::~SmartSerialDetector() {
    stop();
}

bool SmartSerialDetector::start(bool enable_hotplug) {
    if (running_.load()) {
        return true; // 已经在运行
    }

    enable_hotplug_ = enable_hotplug;
    running_.store(true);

    // 启动扫描线程
    scan_thread_ = std::thread(&SmartSerialDetector::scanThread, this);

    // 如果启用热插拔，启动热插拔监控线程
    if (enable_hotplug) {
        hotplug_thread_ = std::thread(&SmartSerialDetector::hotplugThread, this);
    }

    // 执行初始扫描
    scanDevices();

    DEBUG_CORE_LOG("Smart Serial Detector started successfully");
    return true;
}

void SmartSerialDetector::stop() {
    if (!running_.load()) {
        DEBUG_CORE_LOG("Smart Serial Detector already stopped");
        return;
    }

    running_.store(false);
    stop_event_threads_.store(true);

    // 等待线程结束
    if (scan_thread_.joinable()) {
        scan_thread_.join();
    }
    if (hotplug_thread_.joinable()) {
        hotplug_thread_.join();
    }

    // 等待所有事件线程结束
    for (auto& thread : event_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    event_threads_.clear();

    // 清理设备列表
    std::lock_guard<std::mutex> lock(devices_mutex_);
    devices_.clear();

    DEBUG_CORE_LOG("Smart Serial Detector stopped");
}

int SmartSerialDetector::scanDevices() {
    std::vector<std::string> found_devices;
    
    // 扫描常见的串口设备路径
    std::vector<std::string> device_patterns = {
        "/dev/ttyUSB*",
        "/dev/ttyACM*", 
        "/dev/ttyXRUSB*",
        "/dev/ttyXRACM*"
    };

    for (const auto& pattern : device_patterns) {
        // 扫描/dev目录
        DIR* dir = opendir("/dev");
        if (!dir) {
            DEBUG_CORE_LOG("Cannot open /dev directory");
            continue;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            
            // 检查是否匹配模式
            bool matches = false;
            if (pattern.find("ttyUSB") != std::string::npos && name.substr(0, 6) == "ttyUSB") {
                matches = true;
            } else if (pattern.find("ttyACM") != std::string::npos && name.substr(0, 6) == "ttyACM") {
                matches = true;
            } else if (pattern.find("ttyXRUSB") != std::string::npos && name.substr(0, 8) == "ttyXRUSB") {
                matches = true;
            } else if (pattern.find("ttyXRACM") != std::string::npos && name.substr(0, 8) == "ttyXRACM") {
                matches = true;
            }
            
            if (matches) {
                std::string device_path = "/dev/" + name;
                if (isUSBSerialDevice(device_path)) {
                    found_devices.push_back(device_path);
                }
            }
        }
        closedir(dir);
    }

    // 处理找到的设备
    std::lock_guard<std::mutex> lock(devices_mutex_);
    
    // 检查新连接的设备
    for (const auto& device_path : found_devices) {
        if (devices_.find(device_path) == devices_.end()) {
            handleDeviceConnect(device_path);
        }
    }

    // 检查已断开的设备
    auto it = devices_.begin();
    while (it != devices_.end()) {
        if (!checkDeviceExists(it->first)) {
            handleDeviceDisconnect(it->first);
            it = devices_.erase(it);
        } else {
            ++it;
        }
    }

    return static_cast<int>(devices_.size());
}

std::vector<SmartSerialDeviceInfo> SmartSerialDetector::getConnectedDevices() const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    std::vector<SmartSerialDeviceInfo> result;
    
    for (const auto& [path, info] : devices_) {
        if (info && info->is_connected) {
            result.push_back(*info);
        }
    }
    
    return result;
}

int SmartSerialDetector::getConnectedDeviceCount() const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    int count = 0;
    
    for (const auto& [path, info] : devices_) {
        if (info && info->is_connected) {
            count++;
        }
    }
    
    return count;
}

bool SmartSerialDetector::isDeviceConnected(const std::string& device_path) const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = devices_.find(device_path);
    return it != devices_.end() && it->second && it->second->is_connected;
}

std::shared_ptr<SmartSerialDeviceInfo> SmartSerialDetector::getDeviceInfo(const std::string& device_path) const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = devices_.find(device_path);
    if (it != devices_.end()) {
        return it->second;
    }
    return nullptr;
}

void SmartSerialDetector::setDeviceCallback(DeviceCallback callback) {
    device_callback_ = callback;
}

bool SmartSerialDetector::reconnectDevice(const std::string& device_path) {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = devices_.find(device_path);
    if (it != devices_.end() && it->second) {
        auto info = it->second;
        
        // 检查是否可以重连
        auto now = std::chrono::steady_clock::now();
        auto time_since_last_attempt = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - info->last_reconnect_attempt).count();
        
        if (time_since_last_attempt < RECONNECT_INTERVAL_MS) {
            DEBUG_CORE_LOG("Reconnect attempt too soon for device: " << device_path);
            return false;
        }
        
        // 尝试重连
        info->reconnect_attempts++;
        info->last_reconnect_attempt = now;
        
        // 检查设备是否重新出现
        if (checkDeviceExists(device_path)) {
            info->is_connected = true;
            info->last_seen = now;
            info->reconnect_attempts = 0; // 重置重连计数
            
            DEBUG_CORE_LOG("Device reconnected: " << device_path);
            
            if (device_callback_) {
                device_callback_(device_path, true);
            }
            
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> SmartSerialDetector::getAllSerialDevices() const {
    std::vector<std::string> devices;
    
    DIR* dir = opendir("/dev");
    if (!dir) {
        return devices;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        
        // 检查常见的串口设备模式
        if (name.substr(0, 6) == "ttyUSB" || 
            name.substr(0, 6) == "ttyACM" ||
            name.substr(0, 8) == "ttyXRUSB" ||
            name.substr(0, 8) == "ttyXRACM") {
            devices.push_back("/dev/" + name);
        }
    }
    closedir(dir);
    
    return devices;
}

std::vector<std::string> SmartSerialDetector::findDevicesByCharacteristics(
    const std::string& vendor_id,
    const std::string& product_id, 
    const std::string& serial_number) const {
    
    std::vector<std::string> matching_devices;
    std::lock_guard<std::mutex> lock(devices_mutex_);
    
    for (const auto& [path, info] : devices_) {
        if (!info || !info->is_connected) continue;
        
        bool matches = true;
        
        if (!vendor_id.empty() && info->vendor_id != vendor_id) {
            matches = false;
        }
        if (!product_id.empty() && info->product_id != product_id) {
            matches = false;
        }
        if (!serial_number.empty() && info->serial_number != serial_number) {
            matches = false;
        }
        
        if (matches) {
            matching_devices.push_back(path);
        }
    }
    
    return matching_devices;
}

std::string SmartSerialDetector::selectBestDevice(const std::string& preferred_device) const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    
    // 如果指定了首选设备且该设备存在，优先选择
    if (!preferred_device.empty()) {
        auto it = devices_.find(preferred_device);
        if (it != devices_.end() && it->second && it->second->is_connected) {
            DEBUG_CORE_LOG("Selected preferred device: " << preferred_device);
            return preferred_device;
        }
    }
    
    // 根据优先级选择最佳设备
    std::string best_device;
    int best_priority = -1;
    
    for (const auto& [path, info] : devices_) {
        if (!info || !info->is_connected) continue;
        
        // 检查是否应该过滤此设备
        if (vendor_config_ && vendor_config_->shouldFilterDevice(info->driver_name)) {
            continue;
        }
        
        int priority = info->priority; // 使用预计算的优先级
        if (priority > best_priority) {
            best_priority = priority;
            best_device = path;
        }
    }
    
    if (!best_device.empty()) {
        DEBUG_CORE_LOG("Selected best device: " << best_device << " (priority: " << best_priority << ")");
    }
    
    return best_device;
}

void SmartSerialDetector::addDeviceRule(const SerialDeviceRule& rule) {
    std::lock_guard<std::mutex> lock(rules_mutex_);
    device_rules_.push_back(rule);
    DEBUG_CORE_LOG("Added device rule: " << rule.name);
}

void SmartSerialDetector::removeDeviceRule(const std::string& rule_name) {
    std::lock_guard<std::mutex> lock(rules_mutex_);
    auto it = std::find_if(device_rules_.begin(), device_rules_.end(),
        [&rule_name](const SerialDeviceRule& rule) {
            return rule.name == rule_name;
        });
    
    if (it != device_rules_.end()) {
        device_rules_.erase(it);
        DEBUG_CORE_LOG("Removed device rule: " << rule_name);
    }
}

SmartSerialDetector::RuleList SmartSerialDetector::getDeviceRules() const {
    std::lock_guard<std::mutex> lock(rules_mutex_);
    return device_rules_;
}

void SmartSerialDetector::setVendorConfigPath(const std::string& config_path) {
    vendor_config_path_ = config_path;
    if (vendor_config_) {
        vendor_config_->loadConfig(config_path);
        DEBUG_CORE_LOG("USB vendor config loaded from: " << config_path);
    }
}

std::string SmartSerialDetector::getVendorConfigStatus() const {
    if (vendor_config_) {
        return vendor_config_->getConfigStatus();
    }
    return "USB vendor config not initialized";
}

std::string SmartSerialDetector::getDeviceChipType(const std::string& device_path) const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = devices_.find(device_path);
    if (it != devices_.end() && it->second) {
        return it->second->chip_type;
    }
    return "Unknown";
}

bool SmartSerialDetector::isDeviceModbusCompatible(const std::string& device_path) const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = devices_.find(device_path);
    if (it != devices_.end() && it->second) {
        // 所有USB转串口设备都支持Modbus通信
        return it->second->is_connected;
    }
    return false;
}

void SmartSerialDetector::scanThread() {
    while (running_.load()) {
        scanDevices();
        autoReconnectDevices();
        std::this_thread::sleep_for(std::chrono::milliseconds(SCAN_INTERVAL_MS));
    }
}

void SmartSerialDetector::hotplugThread() {
    // 使用inotify监控/sys/class/tty目录的变化
    // 这里简化实现，实际可以使用inotify进行更精确的监控
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(SCAN_INTERVAL_MS));
        
        // 检查是否有新设备连接
        scanDevices();
    }
}

std::shared_ptr<SmartSerialDeviceInfo> SmartSerialDetector::readDeviceInfo(const std::string& device_path) {
    auto info = std::make_shared<SmartSerialDeviceInfo>();
    info->device_path = device_path;
    info->is_connected = false;
    info->last_seen = std::chrono::steady_clock::now();
    info->reconnect_attempts = 0;
    info->last_reconnect_attempt = std::chrono::steady_clock::now();
    info->priority = 0;

    // 尝试打开设备以验证其存在
    int fd = open(device_path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        DEBUG_CORE_LOG("Cannot open device: " << device_path << " - " << strerror(errno));
        return info;
    }
    close(fd);

    // 从sysfs读取USB设备信息
    readUSBInfoFromSysfs(device_path, *info);
    
    // 识别芯片类型
    if (vendor_config_) {
        info->chip_type = vendor_config_->identifyChipType(info->vendor_id, info->product_id, info->driver_name);
    } else {
        info->chip_type = "Unknown";
    }
    
    // 计算优先级
    if (vendor_config_) {
        info->priority = vendor_config_->calculateDevicePriority(info->vendor_id, info->product_id, info->driver_name);
    } else {
        info->priority = 50; // 默认优先级
    }
    
    info->is_connected = true;
    
    DEBUG_CORE_LOG("Smart Serial device detected: " << device_path 
                  << " (" << info->device_name << ", " << info->chip_type << ", priority: " << info->priority << ")");
    
    return info;
}

bool SmartSerialDetector::isUSBSerialDevice(const std::string& device_path) {
    // 检查设备文件是否存在
    struct stat st;
    if (stat(device_path.c_str(), &st) != 0) {
        return false;
    }
    
    // 检查是否为字符设备
    if (!S_ISCHR(st.st_mode)) {
        return false;
    }
    
    // 尝试打开设备
    int fd = open(device_path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        return false;
    }
    close(fd);
    
    return true;
}

void SmartSerialDetector::handleDeviceConnect(const std::string& device_path) {
    auto info = readDeviceInfo(device_path);
    if (info && info->is_connected) {
        devices_[device_path] = info;
        
        // 调用回调函数
        if (device_callback_) {
            device_callback_(device_path, true);
        }
    }
}

void SmartSerialDetector::handleDeviceDisconnect(const std::string& device_path) {
    auto it = devices_.find(device_path);
    if (it != devices_.end()) {
        auto info = it->second;
        if (info) {
            info->is_connected = false;
            
            // 调用回调函数
            if (device_callback_) {
                device_callback_(device_path, false);
            }
        }
    }
}

void SmartSerialDetector::readUSBInfoFromSysfs(const std::string& device_path, SmartSerialDeviceInfo& info) {
    // 从设备路径提取设备名称
    std::string device_name = device_path.substr(device_path.find_last_of('/') + 1);
    
    // 尝试从/sys/class/tty读取设备信息
    std::string sys_path = "/sys/class/tty/" + device_name;
    
    // 读取设备名称
    std::ifstream name_file(sys_path + "/device/name");
    if (name_file.is_open()) {
        std::getline(name_file, info.device_name);
        name_file.close();
    } else {
        info.device_name = device_name;
    }
    
    // 尝试读取USB信息
    std::string usb_path = sys_path + "/device";
    
    // 读取厂商ID
    std::ifstream vendor_file(usb_path + "/idVendor");
    if (vendor_file.is_open()) {
        std::getline(vendor_file, info.vendor_id);
        vendor_file.close();
    }
    
    // 读取产品ID
    std::ifstream product_file(usb_path + "/idProduct");
    if (product_file.is_open()) {
        std::getline(product_file, info.product_id);
        product_file.close();
    }
    
    // 读取序列号
    std::ifstream serial_file(usb_path + "/serial");
    if (serial_file.is_open()) {
        std::getline(serial_file, info.serial_number);
        serial_file.close();
    }
    
    // 读取驱动名称
    std::ifstream driver_file(usb_path + "/driver");
    if (driver_file.is_open()) {
        std::getline(driver_file, info.driver_name);
        driver_file.close();
    }
    
    // 构建USB路径
    info.usb_path = usb_path;
    
    // 如果无法读取USB信息，设置默认值
    if (info.vendor_id.empty()) {
        info.vendor_id = "unknown";
    }
    if (info.product_id.empty()) {
        info.product_id = "unknown";
    }
    if (info.serial_number.empty()) {
        info.serial_number = "unknown";
    }
    if (info.driver_name.empty()) {
        info.driver_name = "unknown";
    }
}

std::string SmartSerialDetector::parseDeviceName(const std::string& raw_name) {
    // 清理设备名称，移除特殊字符
    std::string clean_name = raw_name;
    
    // 移除控制字符
    clean_name.erase(std::remove_if(clean_name.begin(), clean_name.end(),
        [](char c) { return std::iscntrl(c); }), clean_name.end());
    
    // 移除多余的空格
    clean_name = std::regex_replace(clean_name, std::regex("\\s+"), " ");
    
    // 去除首尾空格
    clean_name.erase(0, clean_name.find_first_not_of(" \t"));
    clean_name.erase(clean_name.find_last_not_of(" \t") + 1);
    
    return clean_name.empty() ? "Unknown Device" : clean_name;
}

bool SmartSerialDetector::checkDeviceExists(const std::string& device_path) {
    struct stat st;
    return stat(device_path.c_str(), &st) == 0 && S_ISCHR(st.st_mode);
}

void SmartSerialDetector::autoReconnectDevices() {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto now = std::chrono::steady_clock::now();
    
    for (auto& [path, info] : devices_) {
        if (!info || info->is_connected) continue;
        
        // 检查是否可以尝试重连
        auto time_since_last_attempt = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - info->last_reconnect_attempt).count();
        
        if (time_since_last_attempt >= RECONNECT_INTERVAL_MS) {
            if (checkDeviceExists(path)) {
                info->is_connected = true;
                info->last_seen = now;
                info->reconnect_attempts = 0;
                
                DEBUG_CORE_LOG("Device auto-reconnected: " << path);
                
                if (device_callback_) {
                    device_callback_(path, true);
                }
            } else {
                info->reconnect_attempts++;
                info->last_reconnect_attempt = now;
                
                DEBUG_CORE_LOG("Auto-reconnect attempt " << info->reconnect_attempts 
                              << " for device: " << path);
            }
        }
    }
}

// 硬编码的芯片类型识别方法已移除，现在使用USB厂商配置

// 硬编码的优先级计算方法已移除，现在使用USB厂商配置

bool SmartSerialDetector::matchesRule(const SmartSerialDeviceInfo& device_info, const SerialDeviceRule& rule) const {
    // 检查厂商ID
    if (!rule.vendor_id.empty() && device_info.vendor_id != rule.vendor_id) {
        return false;
    }
    
    // 检查产品ID
    if (!rule.product_id.empty() && device_info.product_id != rule.product_id) {
        return false;
    }
    
    // 检查序列号模式
    if (!rule.serial_pattern.empty()) {
        std::regex pattern(rule.serial_pattern);
        if (!std::regex_search(device_info.serial_number, pattern)) {
            return false;
        }
    }
    
    // 检查驱动名称模式
    if (!rule.driver_pattern.empty()) {
        std::regex pattern(rule.driver_pattern);
        if (!std::regex_search(device_info.driver_name, pattern)) {
            return false;
        }
    }
    
    // 检查设备名称模式
    if (!rule.device_pattern.empty()) {
        std::regex pattern(rule.device_pattern);
        if (!std::regex_search(device_info.device_name, pattern)) {
            return false;
        }
    }
    
    return true;
}

void SmartSerialDetector::initializeDefaultRules() {
    std::lock_guard<std::mutex> lock(rules_mutex_);
    device_rules_.clear();
    
    // FTDI设备规则
    SerialDeviceRule ftdi_rule;
    ftdi_rule.name = "FTDI_Devices";
    ftdi_rule.vendor_id = "0403";
    ftdi_rule.priority = 20;
    ftdi_rule.enabled = true;
    device_rules_.push_back(ftdi_rule);
    
    // CP210x设备规则
    SerialDeviceRule cp210x_rule;
    cp210x_rule.name = "CP210x_Devices";
    cp210x_rule.vendor_id = "10c4";
    cp210x_rule.priority = 15;
    cp210x_rule.enabled = true;
    device_rules_.push_back(cp210x_rule);
    
    // CH34x设备规则
    SerialDeviceRule ch34x_rule;
    ch34x_rule.name = "CH34x_Devices";
    ch34x_rule.vendor_id = "1a86";
    ch34x_rule.priority = 10;
    ch34x_rule.enabled = true;
    device_rules_.push_back(ch34x_rule);
    
    // CDC ACM设备规则
    SerialDeviceRule cdc_rule;
    cdc_rule.name = "CDC_ACM_Devices";
    cdc_rule.driver_pattern = "cdc_acm";
    cdc_rule.priority = 5;
    cdc_rule.enabled = true;
    device_rules_.push_back(cdc_rule);
    
    DEBUG_CORE_LOG("Initialized default device rules");
}
