/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-01-27 11:30:14
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-01-27 11:30:14
 * @FilePath: include/utils/thread_safe_logger.h
 * @Description: 线程安全日志管理器
 * 解决多线程环境下日志颜色输出冲突问题
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */
#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include "../core/constants.h"

/**
 * @brief 线程安全日志管理器
 * 
 * 使用单例模式确保全局只有一个日志管理器实例，
 * 通过互斥锁保证多线程环境下的日志输出原子性，
 * 避免ANSI颜色代码被其他线程输出打断。
 */
class ThreadSafeLogger {
public:
    /**
     * @brief 获取单例实例
     * @return 日志管理器实例的引用
     */
    static ThreadSafeLogger& getInstance() {
        static ThreadSafeLogger instance;
        return instance;
    }

    /**
     * @brief 线程安全的信息日志输出
     * @param msg 日志消息
     */
    void logInfo(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << COLOR_INFO << "[" << NowTimeString() << "] [INFO] " << msg << COLOR_RESET << std::endl;
    }

    /**
     * @brief 线程安全的警告日志输出
     * @param msg 日志消息
     */
    void logWarning(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << COLOR_WARNING << "[" << NowTimeString() << "] [WARN] " << msg << COLOR_RESET << std::endl;
    }

    /**
     * @brief 线程安全的错误日志输出
     * @param msg 日志消息
     */
    void logError(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << COLOR_ERROR << "[" << NowTimeString() << "] [ERROR] " << msg << COLOR_RESET << std::endl;
    }

    /**
     * @brief 线程安全的调试日志输出
     * @param msg 日志消息
     */
    void logDebug(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << COLOR_DEBUG << "[" << NowTimeString() << "] [DEBUG] " << msg << COLOR_RESET << std::endl;
    }

    /**
     * @brief 线程安全的模块调试日志输出
     * @param module_color 模块颜色代码
     * @param module_name 模块名称
     * @param msg 日志消息
     */
    void logModuleDebug(const std::string& module_color, const std::string& module_name, const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << module_color << "[" << NowTimeString() << "] [" << module_name << "] " << msg << COLOR_RESET << std::endl;
    }

    /**
     * @brief 线程安全的模块错误日志输出
     * @param module_color 模块颜色代码
     * @param module_name 模块名称
     * @param msg 日志消息
     */
    void logModuleError(const std::string& module_color, const std::string& module_name, const std::string& msg) {
        (void)module_color; // 保留参数以保持接口一致性，但错误消息始终使用红色
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << COLOR_ERROR << "[" << NowTimeString() << "] [" << module_name << "] " << msg << COLOR_RESET << std::endl;
    }

    /**
     * @brief 线程安全的模块警告日志输出
     * @param module_color 模块颜色代码
     * @param module_name 模块名称
     * @param msg 日志消息
     */
    void logModuleWarning(const std::string& module_color, const std::string& module_name, const std::string& msg) {
        (void)module_color; // 保留参数以保持接口一致性，但警告消息始终使用黄色
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << COLOR_WARNING << "[" << NowTimeString() << "] [" << module_name << "] " << msg << COLOR_RESET << std::endl;
    }

private:
    std::mutex mutex_;  ///< 互斥锁，保护日志输出

    /**
     * @brief 私有构造函数（单例模式）
     */
    ThreadSafeLogger() = default;

    /**
     * @brief 禁用拷贝构造函数
     */
    ThreadSafeLogger(const ThreadSafeLogger&) = delete;

    /**
     * @brief 禁用赋值操作符
     */
    ThreadSafeLogger& operator=(const ThreadSafeLogger&) = delete;

    /**
     * @brief 获取当前时间的格式化字符串
     * @return 格式为"YYYY-MM-DD HH:MM:SS.mmm"的时间字符串
     */
    std::string NowTimeString() {
        using namespace std::chrono;
        auto tp = system_clock::now();
        auto t = system_clock::to_time_t(tp);
        auto ms = duration_cast<milliseconds>(tp.time_since_epoch()) % 1000;
        
        std::tm tm{};
        localtime_r(&t, &tm);
        
        std::ostringstream oss;
        oss << std::put_time(&tm, "%F %T") << '.' << std::setw(3) << std::setfill('0') << ms.count();
        return oss.str();
    }
};

// 便捷的全局访问宏
#define THREAD_SAFE_LOGI(msg) ThreadSafeLogger::getInstance().logInfo(msg)
#define THREAD_SAFE_LOGW(msg) ThreadSafeLogger::getInstance().logWarning(msg)
#define THREAD_SAFE_LOGE(msg) ThreadSafeLogger::getInstance().logError(msg)
#define THREAD_SAFE_LOGD(msg) ThreadSafeLogger::getInstance().logDebug(msg)

// 模块专用线程安全日志宏
#define THREAD_SAFE_DEBUG_CORE_LOG(msg) ThreadSafeLogger::getInstance().logModuleDebug(COLOR_INFO, "CORE", msg)
#define THREAD_SAFE_DEBUG_MODBUS_LOG(msg) ThreadSafeLogger::getInstance().logModuleDebug(COLOR_MODBUS, "ACQUISITION", msg)
#define THREAD_SAFE_DEBUG_JOYSTICK_LOG(msg) ThreadSafeLogger::getInstance().logModuleDebug(COLOR_JOYSTICK, "JOYSTICK", msg)
#define THREAD_SAFE_DEBUG_ROS2_LOG(msg) ThreadSafeLogger::getInstance().logModuleDebug(COLOR_ROS2, "ROS2", msg)
#define THREAD_SAFE_DEBUG_UINPUT_LOG(msg) ThreadSafeLogger::getInstance().logModuleDebug(COLOR_UINPUT, "UINPUT", msg)
#define THREAD_SAFE_DEBUG_CONFIG_LOG(msg) ThreadSafeLogger::getInstance().logModuleDebug(COLOR_CONFIG, "CONFIG", msg)
#define THREAD_SAFE_DEBUG_MAPPING_LOG(msg) ThreadSafeLogger::getInstance().logModuleDebug(COLOR_WARNING, "MAPPING", msg)
#define THREAD_SAFE_DEBUG_UTILS_LOG(msg) ThreadSafeLogger::getInstance().logModuleDebug(COLOR_DEBUG, "UTILS", msg)

// 模块错误和警告日志宏
#define THREAD_SAFE_DEBUG_CORE_ERROR(msg) ThreadSafeLogger::getInstance().logModuleError(COLOR_INFO, "CORE", msg)
#define THREAD_SAFE_DEBUG_CORE_WARNING(msg) ThreadSafeLogger::getInstance().logModuleWarning(COLOR_INFO, "CORE", msg)
#define THREAD_SAFE_DEBUG_MODBUS_ERROR(msg) ThreadSafeLogger::getInstance().logModuleError(COLOR_MODBUS, "ACQUISITION", msg)
#define THREAD_SAFE_DEBUG_MODBUS_WARNING(msg) ThreadSafeLogger::getInstance().logModuleWarning(COLOR_MODBUS, "ACQUISITION", msg)
#define THREAD_SAFE_DEBUG_JOYSTICK_ERROR(msg) ThreadSafeLogger::getInstance().logModuleError(COLOR_JOYSTICK, "JOYSTICK", msg)
#define THREAD_SAFE_DEBUG_JOYSTICK_WARNING(msg) ThreadSafeLogger::getInstance().logModuleWarning(COLOR_JOYSTICK, "JOYSTICK", msg)
#define THREAD_SAFE_DEBUG_ROS2_ERROR(msg) ThreadSafeLogger::getInstance().logModuleError(COLOR_ROS2, "ROS2", msg)
#define THREAD_SAFE_DEBUG_ROS2_WARNING(msg) ThreadSafeLogger::getInstance().logModuleWarning(COLOR_ROS2, "ROS2", msg)
#define THREAD_SAFE_DEBUG_UINPUT_ERROR(msg) ThreadSafeLogger::getInstance().logModuleError(COLOR_UINPUT, "UINPUT", msg)
#define THREAD_SAFE_DEBUG_UINPUT_WARNING(msg) ThreadSafeLogger::getInstance().logModuleWarning(COLOR_UINPUT, "UINPUT", msg)
#define THREAD_SAFE_DEBUG_CONFIG_ERROR(msg) ThreadSafeLogger::getInstance().logModuleError(COLOR_CONFIG, "CONFIG", msg)
#define THREAD_SAFE_DEBUG_CONFIG_WARNING(msg) ThreadSafeLogger::getInstance().logModuleWarning(COLOR_CONFIG, "CONFIG", msg)
#define THREAD_SAFE_DEBUG_MAPPING_ERROR(msg) ThreadSafeLogger::getInstance().logModuleError(COLOR_WARNING, "MAPPING", msg)
#define THREAD_SAFE_DEBUG_MAPPING_WARNING(msg) ThreadSafeLogger::getInstance().logModuleWarning(COLOR_WARNING, "MAPPING", msg)
#define THREAD_SAFE_DEBUG_UTILS_ERROR(msg) ThreadSafeLogger::getInstance().logModuleError(COLOR_DEBUG, "UTILS", msg)
#define THREAD_SAFE_DEBUG_UTILS_WARNING(msg) ThreadSafeLogger::getInstance().logModuleWarning(COLOR_DEBUG, "UTILS", msg)
