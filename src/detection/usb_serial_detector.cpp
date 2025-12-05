/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-01-27 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-01-27 11:29:37
 * @FilePath: src/core/usb_serial_detector.cpp
 * @Description: USB转串口设备检测器实现文件
 * 实现USB转串口设备的自动检测、热插拔监控和自动恢复功能
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "usb_serial_detector.h"
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

USBSerialDetector::USBSerialDetector() 
    : running_(false), enable_hotplug_(false), stop_event_threads_(false) {
}

USBSerialDetector::~USBSerialDetector() {
    stop();
}

bool USBSerialDetector::start(bool enable_hotplug) {
    if (running_.load()) {
        return true; // 已经在运行
    }

    enable_hotplug_ = enable_hotplug;
    running_.store(true);

    // 启动扫描线程
    scan_thread_ = std::thread(&USBSerialDetector::scanThread, this);

    // 如果启用热插拔，启动热插拔监控线程
    if (enable_hotplug) {
        hotplug_thread_ = std::thread(&USBSerialDetector::hotplugThread, this);
    }

    // 执行初始扫描
    scanDevices();

    DEBUG_CORE_LOG("USB 串口检测器已成功启动");
    return true;
}

void USBSerialDetector::stop() {
    if (!running_.load()) {
        DEBUG_CORE_LOG("USB 串口检测器已停止");
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

    DEBUG_CORE_LOG("USB 串口检测器已停止");
}

int USBSerialDetector::scanDevices() {
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
            DEBUG_CORE_LOG("无法打开 /dev 目录");
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

std::vector<USBSerialDeviceInfo> USBSerialDetector::getConnectedDevices() const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    std::vector<USBSerialDeviceInfo> result;
    
    for (const auto& [path, info] : devices_) {
        if (info && info->is_connected) {
            result.push_back(*info);
        }
    }
    
    return result;
}

int USBSerialDetector::getConnectedDeviceCount() const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    int count = 0;
    
    for (const auto& [path, info] : devices_) {
        if (info && info->is_connected) {
            count++;
        }
    }
    
    return count;
}

bool USBSerialDetector::isDeviceConnected(const std::string& device_path) const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = devices_.find(device_path);
    return it != devices_.end() && it->second && it->second->is_connected;
}

std::shared_ptr<USBSerialDeviceInfo> USBSerialDetector::getDeviceInfo(const std::string& device_path) const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = devices_.find(device_path);
    if (it != devices_.end()) {
        return it->second;
    }
    return nullptr;
}

void USBSerialDetector::setDeviceCallback(DeviceCallback callback) {
    device_callback_ = callback;
}

bool USBSerialDetector::reconnectDevice(const std::string& device_path) {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = devices_.find(device_path);
    if (it != devices_.end() && it->second) {
        auto info = it->second;
        
        // 检查是否可以重连
        auto now = std::chrono::steady_clock::now();
        auto time_since_last_attempt = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - info->last_reconnect_attempt).count();
        
        if (time_since_last_attempt < RECONNECT_INTERVAL_MS) {
            DEBUG_CORE_LOG("设备重连尝试过于频繁: " << device_path);
            return false;
        }
        
        // 移除重连次数限制，持续尝试重连
        // if (info->reconnect_attempts >= MAX_RECONNECT_ATTEMPTS) {
        //     DEBUG_CORE_LOG("Max reconnect attempts reached for device: " << device_path);
        //     return false;
        // }
        
        // 尝试重连
        info->reconnect_attempts++;
        info->last_reconnect_attempt = now;
        
        // 检查设备是否重新出现
        if (checkDeviceExists(device_path)) {
            info->is_connected = true;
            info->last_seen = now;
            info->reconnect_attempts = 0; // 重置重连计数
            
            DEBUG_CORE_LOG("设备已重新连接: " << device_path);
            
            if (device_callback_) {
                device_callback_(device_path, true);
            }
            
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> USBSerialDetector::getAllSerialDevices() const {
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

std::vector<std::string> USBSerialDetector::findDevicesByCharacteristics(
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

void USBSerialDetector::scanThread() {
    while (running_.load()) {
        scanDevices();
        autoReconnectDevices();
        std::this_thread::sleep_for(std::chrono::milliseconds(SCAN_INTERVAL_MS));
    }
}

void USBSerialDetector::hotplugThread() {
    // 使用inotify监控/sys/class/tty目录的变化
    // 这里简化实现，实际可以使用inotify进行更精确的监控
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(SCAN_INTERVAL_MS));
        
        // 检查是否有新设备连接
        scanDevices();
    }
}

std::shared_ptr<USBSerialDeviceInfo> USBSerialDetector::readDeviceInfo(const std::string& device_path) {
    auto info = std::make_shared<USBSerialDeviceInfo>();
    info->device_path = device_path;
    info->is_connected = false;
    info->last_seen = std::chrono::steady_clock::now();
    info->reconnect_attempts = 0;
    info->last_reconnect_attempt = std::chrono::steady_clock::now();

    // 尝试打开设备以验证其存在
    int fd = open(device_path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        DEBUG_CORE_LOG("无法打开设备: " << device_path << " - " << strerror(errno));
        return info;
    }
    close(fd);

    // 从sysfs读取USB设备信息
    readUSBInfoFromSysfs(device_path, *info);
    
    info->is_connected = true;
    
    DEBUG_CORE_LOG("检测到 USB 串口设备: " << device_path 
                  << " (" << info->device_name << ")");
    
    return info;
}

bool USBSerialDetector::isUSBSerialDevice(const std::string& device_path) {
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

void USBSerialDetector::handleDeviceConnect(const std::string& device_path) {
    auto info = readDeviceInfo(device_path);
    if (info && info->is_connected) {
        devices_[device_path] = info;
        
        // 调用回调函数
        if (device_callback_) {
            device_callback_(device_path, true);
        }
    }
}

void USBSerialDetector::handleDeviceDisconnect(const std::string& device_path) {
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

void USBSerialDetector::readUSBInfoFromSysfs(const std::string& device_path, USBSerialDeviceInfo& info) {
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
}

std::string USBSerialDetector::parseDeviceName(const std::string& raw_name) {
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

bool USBSerialDetector::checkDeviceExists(const std::string& device_path) {
    struct stat st;
    return stat(device_path.c_str(), &st) == 0 && S_ISCHR(st.st_mode);
}

void USBSerialDetector::autoReconnectDevices() {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto now = std::chrono::steady_clock::now();
    
    for (auto& [path, info] : devices_) {
        if (!info || info->is_connected) continue;
        
        // 检查是否可以尝试重连
        auto time_since_last_attempt = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - info->last_reconnect_attempt).count();
        
        if (time_since_last_attempt >= RECONNECT_INTERVAL_MS) {
            // 移除重连次数限制，持续尝试重连
            // && info->reconnect_attempts < MAX_RECONNECT_ATTEMPTS) {
            
            if (checkDeviceExists(path)) {
                info->is_connected = true;
                info->last_seen = now;
                info->reconnect_attempts = 0;
                
                DEBUG_CORE_LOG("设备已自动重新连接: " << path);
                
                if (device_callback_) {
                    device_callback_(path, true);
                }
            } else {
                info->reconnect_attempts++;
                info->last_reconnect_attempt = now;
                
                DEBUG_CORE_LOG("设备自动重连尝试 " << info->reconnect_attempts 
                              << " 次: " << path);
            }
        }
    }
}
