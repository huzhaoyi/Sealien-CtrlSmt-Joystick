/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-09-08 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-09-08 11:29:37
 * @FilePath: src/core/joystick_manager.cpp
 * @Description: USB手柄管理器实现文件
 * 负责管理USB物理手柄的检测、事件处理和ROS2消息发布
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "joystick_manager.h"
#include <iomanip>
#include <sstream>
#include "core/constants.h"
#include "utils/utils.h"
#include "utils/filters.h"

#include <algorithm>

JoystickManager::JoystickManager() {
    joystick_detector_ = std::make_unique<JoystickDetector>();
    timer_interval_ms_.store(USB_JOYSTICK_TIMER_INTERVAL_MS);
}

JoystickManager::~JoystickManager() {
    stop();
}

bool JoystickManager::initialize(bool enable_ros2, 
#ifdef ENABLE_ROS2
                                rclcpp::Node::SharedPtr ros2_node
#else
                                void* ros2_node
#endif
                                ) {
    try {
        enable_ros2_ = enable_ros2;
        
#ifdef ENABLE_ROS2
        if (enable_ros2_ && ros2_node) {
            ros2_node_ = ros2_node;
            initializeROSPublishers();
        }
#endif

        initialized_ = true;
        return true;
    } catch (const std::exception& e) {
        DEBUG_CORE_LOG("摇杆管理器初始化失败: " << e.what());
        return false;
    }
}

void JoystickManager::setAxisMapping(int linear_x_axis, int linear_y_axis, int linear_z_axis,
                                    int angular_x_axis, int angular_y_axis, int angular_z_axis) {
    axis_mapping_.linear_x = linear_x_axis;
    axis_mapping_.linear_y = linear_y_axis;
    axis_mapping_.linear_z = linear_z_axis;
    axis_mapping_.angular_x = angular_x_axis;
    axis_mapping_.angular_y = angular_y_axis;
    axis_mapping_.angular_z = angular_z_axis;
    
    DEBUG_CORE_LOG("轴映射已设置: linear_x=" << linear_x_axis 
                  << ", linear_y=" << linear_y_axis 
                  << ", linear_z=" << linear_z_axis
                  << ", angular_x=" << angular_x_axis 
                  << ", angular_y=" << angular_y_axis 
                  << ", angular_z=" << angular_z_axis);
}

bool JoystickManager::start() {
    if (!initialized_) {
        DEBUG_CORE_LOG("摇杆管理器未初始化");
        return false;
    }

    // 设置手柄事件回调函数
    joystick_detector_->setEventCallback([this](const std::string& device_path, int event_type, int code, int value) {
        onJoystickEvent(device_path, event_type, code, value);
    });

    // 启动手柄检测器
    if (!joystick_detector_->start(true)) {
        DEBUG_JOYSTICK_LOG("无法启动摇杆检测器");
        return false;
    }

    DEBUG_JOYSTICK_LOG("摇杆检测器已成功启动");

    // 扫描当前连接的手柄
    int device_count = joystick_detector_->scanDevices();
    DEBUG_JOYSTICK_LOG("找到 " << device_count << " 个物理摇杆设备");

    // 显示已连接手柄的信息
    auto devices = joystick_detector_->getConnectedDevices();
    for (const auto& device : devices) {
        DEBUG_JOYSTICK_LOG("设备: " << device.device_path 
                         << " 名称: " << device.device_name
                         << " 轴数: " << device.num_axes 
                         << " 按钮数: " << device.num_buttons);
    }

    // 启动定时发送线程
    timer_running_.store(true);
    timer_thread_ = std::thread(&JoystickManager::timerThread, this);
    DEBUG_JOYSTICK_LOG("定时器线程已启动，间隔: " << timer_interval_ms_.load() << "ms");

    return true;
}

void JoystickManager::stop() {
    // 停止定时发送线程
    timer_running_.store(false);
    if (timer_thread_.joinable()) {
        timer_thread_.join();
        DEBUG_JOYSTICK_LOG("定时器线程已停止");
    }
    
    if (joystick_detector_) {
        joystick_detector_->stop();
        DEBUG_JOYSTICK_LOG("摇杆检测器已停止");
    }
}

void JoystickManager::setEventCallback(EventCallback callback) {
    event_callback_ = callback;
}

int JoystickManager::getConnectedDeviceCount() const {
    if (!joystick_detector_) return 0;
    return joystick_detector_->getConnectedDevices().size();
}

std::vector<PhysicalJoystickInfo> JoystickManager::getConnectedDevices() const {
    if (!joystick_detector_) return {};
    return joystick_detector_->getConnectedDevices();
}

bool JoystickManager::isRunning() const {
    return joystick_detector_ && joystick_detector_->isRunning();
}

void JoystickManager::pause() {
    paused_.store(true);
    DEBUG_JOYSTICK_LOG("摇杆管理器已暂停 - USB 摇杆数据处理已禁用");
}

void JoystickManager::resume() {
    paused_.store(false);
    DEBUG_JOYSTICK_LOG("摇杆管理器已恢复 - USB 摇杆数据处理已启用");
}

bool JoystickManager::shouldProcessDevice(const std::string& device_path) const {
    // 过滤掉虚拟设备和不需要的设备
    if (device_path.find("Virtual") != std::string::npos ||
        device_path.find("UInput") != std::string::npos ||
        device_path.find("VMware") != std::string::npos ||
        device_path.find("cfg") != std::string::npos) {
        return false;
    }
    
    // 只处理真正的物理手柄设备
    return device_path.find("/dev/input/js") == 0;
}

void JoystickManager::setTimerInterval(int interval_ms) {
    // 限制定时器间隔在合理范围内
    int clamped_interval = std::clamp(interval_ms, 
                                    USB_JOYSTICK_TIMER_MIN_INTERVAL_MS, 
                                    USB_JOYSTICK_TIMER_MAX_INTERVAL_MS);
    timer_interval_ms_.store(clamped_interval);
    DEBUG_JOYSTICK_LOG("定时器间隔已设置为: " << clamped_interval << "ms");
}

void JoystickManager::onJoystickEvent(const std::string& device_path, int event_type, int code, int value) {
    // 检查是否处于暂停状态
    if (paused_.load()) {
        // DEBUG_JOYSTICK_LOG("JoystickManager is paused, ignoring event from " << device_path);
        return;
    }

    // 检查设备是否应该被处理
    if (!shouldProcessDevice(device_path)) {
        DEBUG_JOYSTICK_LOG("设备已被过滤，忽略来自 " << device_path << " 的事件");
        return;
    }

    switch (event_type) {
        case JoystickDetector::EVENT_CONNECT: {
            DEBUG_JOYSTICK_LOG("物理摇杆已连接: " << device_path);
#ifdef ENABLE_ROS2
            handleDeviceConnect(device_path);
#endif
            break;
        }
        case JoystickDetector::EVENT_DISCONNECT: {
            DEBUG_JOYSTICK_LOG("物理摇杆已断开: " << device_path);
#ifdef ENABLE_ROS2
            handleDeviceDisconnect(device_path);
#endif
            break;
        }
        case JoystickDetector::EVENT_AXIS: {
            // 减少轴事件日志输出频率（只在值变化较大时输出）
            static std::map<std::string, int> last_axis_values;
            int last_value = last_axis_values[device_path + "_" + std::to_string(code)];
            if (abs(value - last_value) > 1000) {  // 只在值变化超过1000时输出日志
                DEBUG_JOYSTICK_LOG("来自 " << device_path << " 的轴事件: axis=" << code << " value=" << value);
                last_axis_values[device_path + "_" + std::to_string(code)] = value;
            }
#ifdef ENABLE_ROS2
            // 只更新数据缓存，不立即发布消息（由定时器统一发布）
            updateAxisData(device_path, code, value);
#endif
            break;
        }
        case JoystickDetector::EVENT_BUTTON: {
            DEBUG_JOYSTICK_LOG("来自 " << device_path << " 的按钮事件: button=" << code << " value=" << value);
#ifdef ENABLE_ROS2
            // 只更新数据缓存，不立即发布消息（由定时器统一发布）
            updateButtonData(device_path, code, value);
#endif
            break;
        }
    }

    // 调用外部回调函数
    if (event_callback_) {
        event_callback_(device_path, event_type, code, value);
    }
}

#ifdef ENABLE_ROS2
void JoystickManager::initializeROSPublishers() {
    if (!ros2_node_) return;

    // 创建ROS2发布者
    joy_pub_ = ros2_node_->create_publisher<sensor_msgs::msg::Joy>(ROS2_TOPIC_JOY, ROS2_TOPIC_QUEUE_SIZE);
    twist_pub_ = ros2_node_->create_publisher<geometry_msgs::msg::Twist>(ROS2_TOPIC_TWIST, ROS2_TOPIC_QUEUE_SIZE);
    status_pub_ = ros2_node_->create_publisher<std_msgs::msg::String>(ROS2_TOPIC_STATUS, ROS2_TOPIC_QUEUE_SIZE);

    DEBUG_ROS2_COMM_LOG("ROS2 publishers created: " << ROS2_TOPIC_JOY << ", " << ROS2_TOPIC_TWIST << ", " << ROS2_TOPIC_STATUS);
}

void JoystickManager::handleDeviceConnect(const std::string& device_path) {
    // 发布ROS2设备连接状态话题
    publishDeviceStatus("CONNECTED", device_path);

    // 初始化设备数据缓存 - 统一格式：12个轴，32个按钮
    {
        std::lock_guard<std::mutex> lock(device_data_mutex_);
        // 初始化12个轴：前4个为摇杆，后8个为旋钮
        std::vector<float> axes_cache(DEFAULT_AXES_CACHE_SIZE, 0.0f);
        
        // 为XBOX手柄的触发器轴设置正确的默认值（如果存在）
        // axis=2和axis=5是XBOX手柄的触发器，默认值应该是-1.0
        if (axes_cache.size() > 2) axes_cache[2] = -1.0f;
        if (axes_cache.size() > 5) axes_cache[5] = -1.0f;
        
        device_axes_[device_path] = axes_cache;
        device_buttons_[device_path] = std::vector<int>(DEFAULT_BUTTONS_CACHE_SIZE, 0);
        
        // 初始化设备状态缓存
        auto device_state = std::make_unique<DeviceState>();
        device_state->axes = axes_cache;
        device_state->buttons = std::vector<int>(DEFAULT_BUTTONS_CACHE_SIZE, 0);
        device_state->last_update = std::chrono::steady_clock::now();
        device_states_[device_path] = std::move(device_state);
    }
}

void JoystickManager::handleDeviceDisconnect(const std::string& device_path) {
    // 发布ROS2设备断开状态话题
    publishDeviceStatus("DISCONNECTED", device_path);

    // 清理设备数据缓存
    {
        std::lock_guard<std::mutex> lock(device_data_mutex_);
        device_axes_.erase(device_path);
        device_buttons_.erase(device_path);
        device_states_.erase(device_path);
    }
}

void JoystickManager::updateAxisData(const std::string& device_path, int axis, int value) {
    // 应用死区处理，避免轻微触碰就产生数据
    double normalized_value = static_cast<double>(value) / AXIS_NORMALIZATION_FACTOR;
    double deadzone_threshold = USB_JOYSTICK_DEFAULT_DEADZONE; // 使用宏定义的死区阈值
    double filtered_value = apply_deadzone(normalized_value, deadzone_threshold);

    // 更新轴数据缓存
    {
        std::lock_guard<std::mutex> lock(device_data_mutex_);
        if (device_axes_.find(device_path) != device_axes_.end() && axis < MAX_JOYSTICK_AXES) {
            device_axes_[device_path][axis] = static_cast<float>(filtered_value);
        }
    }
}

void JoystickManager::updateButtonData(const std::string& device_path, int button, int value) {
    // 更新按钮数据缓存
    {
        std::lock_guard<std::mutex> lock(device_data_mutex_);
        if (device_buttons_.find(device_path) != device_buttons_.end() && button < MAX_JOYSTICK_BUTTONS) {
            device_buttons_[device_path][button] = value;
        }
    }
}

void JoystickManager::publishJoyMessage(const std::string& device_path) {
    if (!joy_pub_) return;

    auto joy_msg = sensor_msgs::msg::Joy();
    joy_msg.header.stamp = ros2_node_->now();
    joy_msg.header.frame_id = device_path;

    // 设置轴和按钮数据
    {
        std::lock_guard<std::mutex> lock(device_data_mutex_);
        if (device_axes_.find(device_path) != device_axes_.end()) {
            joy_msg.axes = device_axes_[device_path];
            joy_msg.buttons = device_buttons_[device_path];
        }
    }

    joy_pub_->publish(joy_msg);
    DEBUG_ROS2_PUBLISH_LOG("已为设备发布 Joy 消息: " << device_path);
    
    // 轴数据打印（通过DEBUG_ROS2_AXES控制）
    if (joy_msg.axes.size() > 0) {
        std::ostringstream axes_oss;
        axes_oss << "ROS2[" << device_path << "] 轴:[";
        for (int i = 0; i < MAX_JOYSTICK_AXES && i < static_cast<int>(joy_msg.axes.size()); i++) {
            if (i > 0) axes_oss << ",";
            axes_oss << std::fixed << std::setprecision(2) << joy_msg.axes[i];
        }
        axes_oss << "]";
        DEBUG_ROS2_AXES_LOG(axes_oss.str());
    }
    
    // 按钮数据打印（通过DEBUG_ROS2_BUTTONS控制）
    if (joy_msg.buttons.size() > 0) {
        std::ostringstream buttons_oss;
        buttons_oss << "ROS2[" << device_path << "] 按钮:[";
        for (int i = 0; i < MAX_JOYSTICK_BUTTONS && i < static_cast<int>(joy_msg.buttons.size()); i++) {
            if (i > 0) buttons_oss << ",";
            buttons_oss << joy_msg.buttons[i];
        }
        buttons_oss << "]";
        DEBUG_ROS2_BUTTONS_LOG(buttons_oss.str());
    }
}

void JoystickManager::publishTwistMessage(const std::string& device_path) {
    if (!twist_pub_) return;

    auto twist_msg = geometry_msgs::msg::Twist();

    // 获取轴数据用于Twist计算
    std::vector<float> axes_data;
    {
        std::lock_guard<std::mutex> lock(device_data_mutex_);
        if (device_axes_.find(device_path) != device_axes_.end()) {
            axes_data = device_axes_[device_path];
        }
    }

    // 使用配置的轴映射设置Twist消息
    if (axis_mapping_.linear_x >= 0 && axis_mapping_.linear_x < static_cast<int>(axes_data.size())) {
        twist_msg.linear.x = axes_data[axis_mapping_.linear_x];
    }
    if (axis_mapping_.linear_y >= 0 && axis_mapping_.linear_y < static_cast<int>(axes_data.size())) {
        twist_msg.linear.y = axes_data[axis_mapping_.linear_y];
    }
    if (axis_mapping_.linear_z >= 0 && axis_mapping_.linear_z < static_cast<int>(axes_data.size())) {
        twist_msg.linear.z = axes_data[axis_mapping_.linear_z];
    }
    if (axis_mapping_.angular_x >= 0 && axis_mapping_.angular_x < static_cast<int>(axes_data.size())) {
        twist_msg.angular.x = axes_data[axis_mapping_.angular_x];
    }
    if (axis_mapping_.angular_y >= 0 && axis_mapping_.angular_y < static_cast<int>(axes_data.size())) {
        twist_msg.angular.y = axes_data[axis_mapping_.angular_y];
    }
    if (axis_mapping_.angular_z >= 0 && axis_mapping_.angular_z < static_cast<int>(axes_data.size())) {
        twist_msg.angular.z = axes_data[axis_mapping_.angular_z];
    }

    twist_pub_->publish(twist_msg);
    DEBUG_ROS2_PUBLISH_LOG("已为设备发布 Twist 消息: " << device_path);
    
    // Twist数据打印（通过DEBUG_ROS2_TWIST控制）
    std::ostringstream twist_oss;
    twist_oss << "ROS2[" << device_path << "] 速度: 线性=(" 
              << std::fixed << std::setprecision(2) << twist_msg.linear.x << ","
              << std::fixed << std::setprecision(2) << twist_msg.linear.y << ","
              << std::fixed << std::setprecision(2) << twist_msg.linear.z << ") 角速度=("
              << std::fixed << std::setprecision(2) << twist_msg.angular.x << ","
              << std::fixed << std::setprecision(2) << twist_msg.angular.y << ","
              << std::fixed << std::setprecision(2) << twist_msg.angular.z << ")";
    DEBUG_ROS2_TWIST_LOG(twist_oss.str());
}

void JoystickManager::publishDeviceStatus(const std::string& status, const std::string& device_path) {
    if (!status_pub_) return;

    auto status_msg = std_msgs::msg::String();
    status_msg.data = status + ":" + device_path;
    status_pub_->publish(status_msg);
    DEBUG_ROS2_PUBLISH_LOG("已发布状态消息: " << status_msg.data);
    
    // 状态数据打印（通过DEBUG_ROS2_STATUS控制）
    DEBUG_ROS2_STATUS_LOG("ROS2[" << device_path << "] Status: " << status);
}

void JoystickManager::timerThread() {
    DEBUG_JOYSTICK_LOG("定时器线程已启动");
    
    while (timer_running_.load()) {
        auto start_time = std::chrono::steady_clock::now();
        
        // 检查是否处于暂停状态
        if (!paused_.load()) {
            // 获取所有连接的设备
            auto devices = joystick_detector_->getConnectedDevices();
            
            // 为每个设备发布消息
            for (const auto& device : devices) {
                if (shouldProcessDevice(device.device_path)) {
                    publishJoyMessage(device.device_path);
                    publishTwistMessage(device.device_path);
                }
            }
        }
        
        // 计算睡眠时间
        auto elapsed = std::chrono::steady_clock::now() - start_time;
        auto sleep_duration = std::chrono::milliseconds(timer_interval_ms_.load()) - elapsed;
        
        if (sleep_duration > std::chrono::milliseconds(0)) {
            std::this_thread::sleep_for(sleep_duration);
        }
    }
    
    DEBUG_JOYSTICK_LOG("定时器线程已停止");
}

void JoystickManager::readDeviceCurrentState(const std::string& device_path) {
    // 这个方法可以用于主动读取设备状态，但目前我们依赖事件驱动
    // 如果需要主动轮询，可以在这里实现
    DEBUG_JOYSTICK_LOG("正在读取设备当前状态: " << device_path);
}
#endif
