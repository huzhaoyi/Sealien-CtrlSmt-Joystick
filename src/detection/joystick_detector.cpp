/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:29:37
 * @FilePath: src/joystick_detector.cpp
 * @Description: USB物理手柄检测器实现文件
 * 实现USB物理手柄的自动检测、热插拔监控和事件读取功能
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "joystick_detector.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <cstring>
#include <algorithm>
#include <mutex>

// Linux输入子系统相关常量
#define JS_EVENT_BUTTON         0x01    // 按钮事件
#define JS_EVENT_AXIS           0x02    // 轴事件
// 手柄事件类型已在constants.h中定义

JoystickDetector::JoystickDetector() {
    // 构造函数，初始化成员变量
}

JoystickDetector::~JoystickDetector() {
    stop();
}

bool JoystickDetector::start(bool enable_hotplug) {
    if (running_.load()) {
        return true; // 已经在运行
    }

    enable_hotplug_ = enable_hotplug;
    running_.store(true);

    // 启动扫描线程
    scan_thread_ = std::thread(&JoystickDetector::scanThread, this);

    // 如果启用热插拔，启动热插拔监控线程
    if (enable_hotplug) {
        hotplug_thread_ = std::thread(&JoystickDetector::hotplugThread, this);
    }

    // 执行初始扫描
    scanDevices();

    return true;
}

void JoystickDetector::stop() {
    if (!running_.load()) {
        DEBUG_JOYSTICK_LOG("摇杆检测器已停止");
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
}

int JoystickDetector::scanDevices() {
    std::vector<std::string> found_devices;
    
    // 扫描/dev/input目录下的所有设备
    DIR* dir = opendir("/dev/input");
    if (!dir) {
        DEBUG_JOYSTICK_LOG("无法打开 /dev/input 目录");
        return 0;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        
        // 检查是否为js设备（joystick）
        if (name.substr(0, 2) == "js") {
            std::string device_path = "/dev/input/" + name;
            if (isJoystickDevice(device_path)) {
                found_devices.push_back(device_path);
            }
        }
    }
    closedir(dir);

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
        if (std::find(found_devices.begin(), found_devices.end(), it->first) == found_devices.end()) {
            handleDeviceDisconnect(it->first);
            it = devices_.erase(it);
        } else {
            ++it;
        }
    }

    return found_devices.size();
}

std::vector<PhysicalJoystickInfo> JoystickDetector::getConnectedDevices() const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    std::vector<PhysicalJoystickInfo> result;
    
    for (const auto& pair : devices_) {
        if (pair.second && pair.second->is_connected) {
            result.push_back(*pair.second);
        }
    }
    
    return result;
}

std::shared_ptr<PhysicalJoystickInfo> JoystickDetector::getDeviceInfo(const std::string& device_path) const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = devices_.find(device_path);
    if (it != devices_.end()) {
        return it->second;
    }
    return nullptr;
}

void JoystickDetector::setEventCallback(JoystickEventCallback callback) {
    event_callback_ = callback;
}

bool JoystickDetector::isDeviceConnected(const std::string& device_path) const {
    std::lock_guard<std::mutex> lock(devices_mutex_);
    auto it = devices_.find(device_path);
    return (it != devices_.end() && it->second && it->second->is_connected);
}

void JoystickDetector::setScanInterval(int interval_ms) {
            scan_interval_ms_ = std::max(JOYSTICK_SCAN_MIN_INTERVAL_MS, interval_ms); // 最小JOYSTICK_SCAN_MIN_INTERVAL_MS
}

void JoystickDetector::scanThread() {
    while (running_.load()) {
        scanDevices();
        
        // 等待下一次扫描
        for (int i = 0; i < scan_interval_ms_ && running_.load(); i += 10) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void JoystickDetector::hotplugThread() {
    // 监控/sys/class/input目录的变化
    // 这里简化实现，实际可以使用inotify进行更精确的监控
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(JOYSTICK_SCAN_INTERVAL_MS));
        
        // 检查是否有新设备连接
        scanDevices();
    }
}

std::shared_ptr<PhysicalJoystickInfo> JoystickDetector::readDeviceInfo(const std::string& device_path) {
    auto info = std::make_shared<PhysicalJoystickInfo>();
    info->device_path = device_path;
    info->is_connected = false;

    int fd = open(device_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        return info;
    }

    // 读取设备名称
    char name[256];
    if (ioctl(fd, JSIOCGNAME(sizeof(name)), name) >= 0) {
        info->device_name = parseDeviceName(name);
    }

    // 读取轴数量
    uint8_t num_axes;
    if (ioctl(fd, JSIOCGAXES, &num_axes) >= 0) {
        info->num_axes = num_axes;
    }

    // 读取按钮数量
    uint8_t num_buttons;
    if (ioctl(fd, JSIOCGBUTTONS, &num_buttons) >= 0) {
        info->num_buttons = num_buttons;
    }

    // 首先尝试通过符号链接找到实际的设备路径
    std::string device_name = device_path.substr(device_path.find_last_of('/') + 1);
    std::string sys_path = "/sys/class/input/" + device_name;
    
    // 检查符号链接是否存在，如果存在则解析实际路径
    char link_target[256];
    ssize_t len = readlink(sys_path.c_str(), link_target, sizeof(link_target) - 1);
    if (len > 0) {
        link_target[len] = '\0';
        // 从链接目标中提取设备路径
        std::string link_str(link_target);
        
        // 统一处理所有设备路径，提取input ID
        size_t input_pos = link_str.find("/input/input");
        if (input_pos != std::string::npos) {
            size_t start = input_pos + 12; // "/input/input" 的长度
            size_t end = link_str.find('/', start);
            std::string input_id;
            if (end != std::string::npos) {
                input_id = link_str.substr(start, end - start);
            } else {
                input_id = link_str.substr(start);
            }
            
            // 判断是虚拟设备还是物理设备
            if (link_str.find("/devices/virtual/input/input") != std::string::npos) {
                // 虚拟设备
                sys_path = "/sys/devices/virtual/input/input" + input_id;
            } else {
                // 物理设备，构建完整的物理设备路径，去掉开头的 "../.."
                std::string device_path_part = link_str.substr(0, input_pos);
                if (device_path_part.find("../..") == 0) {
                    device_path_part = device_path_part.substr(6); // 去掉 "../.."
                }
                sys_path = "/sys/" + device_path_part + "/input/input" + input_id;
            }
        }
    }
    
    
    // 读取厂商ID
    std::ifstream vendor_file(sys_path + "/id/vendor");
    if (vendor_file.is_open()) {
        std::string vendor_str;
        std::getline(vendor_file, vendor_str);
        if (!vendor_str.empty()) {
            info->vendor_id = std::stoi(vendor_str, nullptr, 16);
        } else {
            DEBUG_JOYSTICK_WARNING("设备厂商 ID 为空: " << device_path);
        }
        vendor_file.close();
    } else {
        DEBUG_JOYSTICK_WARNING("无法读取设备厂商 ID: " << device_path);
    }

    // 读取产品ID
    std::ifstream product_file(sys_path + "/id/product");
    if (product_file.is_open()) {
        std::string product_str;
        std::getline(product_file, product_str);
        if (!product_str.empty()) {
            info->product_id = std::stoi(product_str, nullptr, 16);
        } else {
            DEBUG_JOYSTICK_WARNING("设备产品 ID 为空: " << device_path);
        }
        product_file.close();
    } else {
        DEBUG_JOYSTICK_WARNING("Failed to read product ID for device: " << device_path);
    }

    // 读取版本号
    std::ifstream version_file(sys_path + "/id/version");
    if (version_file.is_open()) {
        std::string version_str;
        std::getline(version_file, version_str);
        if (!version_str.empty()) {
            info->version = std::stoi(version_str, nullptr, 16);
        } else {
            DEBUG_JOYSTICK_WARNING("Empty version for device: " << device_path);
        }
        version_file.close();
    } else {
        DEBUG_JOYSTICK_WARNING("Failed to read version for device: " << device_path);
    }

    // 判断设备类型
    info->is_joystick = (info->num_axes > 0 || info->num_buttons > 0);
    info->is_gamepad = (info->num_axes >= 2 && info->num_buttons >= 4);
    info->is_connected = true;

    close(fd);
    return info;
}

bool JoystickDetector::isJoystickDevice(const std::string& device_path) {
    int fd = open(device_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        DEBUG_JOYSTICK_ERROR("Failed to open device: " << device_path << " - " << strerror(errno));
        return false;
    }

    // 检查是否为joystick设备
    uint8_t num_axes, num_buttons;
    bool is_joystick = false;
    
    if (ioctl(fd, JSIOCGAXES, &num_axes) >= 0 && 
        ioctl(fd, JSIOCGBUTTONS, &num_buttons) >= 0) {
        is_joystick = (num_axes > 0 || num_buttons > 0);
    }

    close(fd);
    return is_joystick;
}

void JoystickDetector::handleDeviceConnect(const std::string& device_path) {
    auto info = readDeviceInfo(device_path);
    if (info && info->is_connected) {
        devices_[device_path] = info;
        
        // 启动事件读取线程
        if (!stop_event_threads_.load()) {
            event_threads_.emplace_back(&JoystickDetector::readDeviceEvents, this, device_path);
        }

        // 调用回调函数
        if (event_callback_) {
            event_callback_(device_path, EVENT_CONNECT, 0, 0);
        }

        DEBUG_JOYSTICK_LOG("Joystick connected: " << device_path << " (" << info->device_name << ")");
    }
}

void JoystickDetector::handleDeviceDisconnect(const std::string& device_path) {
    auto it = devices_.find(device_path);
    if (it != devices_.end()) {
        auto info = it->second;
        if (info) {
            info->is_connected = false;
            
            // 调用回调函数
            if (event_callback_) {
                event_callback_(device_path, EVENT_DISCONNECT, 0, 0);
            }

            DEBUG_JOYSTICK_LOG("Joystick disconnected: " << device_path << " (" << info->device_name << ")");
        }
    }
}

void JoystickDetector::readDeviceEvents(const std::string& device_path) {
    int fd = open(device_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        return;
    }

    struct js_event event;
    while (!stop_event_threads_.load()) {
        ssize_t bytes = read(fd, &event, sizeof(event));
        if (bytes == sizeof(event)) {
            if (event.type & JS_EVENT_INIT) {
                // 初始化事件，跳过
                continue;
            }

            if (event_callback_) {
                int event_type = (event.type & JS_EVENT_BUTTON) ? EVENT_BUTTON : EVENT_AXIS;
                event_callback_(device_path, event_type, event.number, event.value);
            }
        } else if (bytes < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                // 读取错误，退出线程
                break;
            }
        }
        
        // 短暂休眠避免CPU占用过高
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    close(fd);
}

std::string JoystickDetector::parseDeviceName(const std::string& name_str) {
    std::string result = name_str;
    
    // 移除末尾的空字符
    size_t null_pos = result.find('\0');
    if (null_pos != std::string::npos) {
        result.erase(null_pos);
    }
    
    // 如果名称为空，使用默认名称
    if (result.empty()) {
        result = "Unknown Joystick";
    }
    
    return result;
}
