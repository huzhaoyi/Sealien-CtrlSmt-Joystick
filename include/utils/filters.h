/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 13:48:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 13:48:37
 * @FilePath: include/filters.h
 * @Description: 信号滤波和去抖头文件
 * 定义低通滤波器、死区处理和去抖器类，用于改善输入信号质量
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#pragma once
#include <algorithm>
#include <chrono>
#include <cmath>
#include "constants.h"

/**
 * @brief 低通滤波器类
 * 使用一阶低通滤波算法平滑输入信号，减少噪声和抖动
 */
class LowPassFilter {
public:
    /**
     * @brief 构造函数
     * @param alpha 滤波系数，范围[0.0, 1.0]，值越小滤波越强
     */
    explicit LowPassFilter(double alpha=1.0): alpha_(std::clamp(alpha,MIN_LPF_ALPHA,MAX_LPF_ALPHA)), y_(0.0), initialized_(false) {}
    
    /**
     * @brief 设置滤波系数
     * @param a 新的滤波系数，会被限制在[0.0, 1.0]范围内
     */
    void setAlpha(double a){ alpha_ = std::clamp(a, MIN_LPF_ALPHA, MAX_LPF_ALPHA); }
    
    /**
     * @brief 处理输入信号
     * @param x 输入值
     * @return 滤波后的输出值
     */
    double process(double x){
        if(!initialized_){ y_ = x; initialized_ = true; return y_; }
        y_ += alpha_ * (x - y_);  // 一阶低通滤波公式：y[n] = y[n-1] + α*(x[n] - y[n-1])
        return y_;
    }
    
    /**
     * @brief 获取当前滤波输出值
     * @return 当前滤波器的输出值
     */
    double value() const { return y_; }
    
    /**
     * @brief 重置滤波器状态
     * @param y 重置后的输出值，默认为0.0
     */
    void reset(double y=0.0){ y_ = y; initialized_=false; }
    
private:
    double alpha_;        ///< 滤波系数，控制滤波强度
    double y_;            ///< 当前滤波输出值
    bool initialized_;    ///< 是否已初始化
};

/**
 * @brief 应用死区处理
 * @param x 输入值，范围[-1.0, 1.0]
 * @param deadzone 死区大小，范围[0.0, 0.49]
 * @return 死区处理后的值，死区内返回0.0，死区外重新映射到[-1.0, 1.0]
 */
inline double apply_deadzone(double x, double deadzone) {
    double dz = std::clamp(deadzone, MIN_DEADZONE, MAX_DEADZONE);  // 限制死区大小，避免完全阻塞信号
    if (std::fabs(x) < dz) return 0.0;  // 死区内返回0
    
    // 死区外重新映射：保持符号，缩放剩余范围
    double sign = (x >= 0.0) ? 1.0 : -1.0;
    return sign * ( (std::fabs(x) - dz) / (1.0 - dz) );
}

/**
 * @brief 去抖器类
 * 防止按钮等数字信号因机械抖动导致的误触发
 */
class Debouncer {
public:
    /**
     * @brief 构造函数
     * @param debounce_ms 去抖时间（毫秒），默认10ms
     */
    explicit Debouncer(int debounce_ms=DEFAULT_DEBOUNCE_MS)
        : debounce_(debounce_ms), stable_(0), last_raw_(0), last_change_(std::chrono::steady_clock::now()) {}
    
    /**
     * @brief 更新去抖器状态
     * @param raw 原始输入值（0或1）
     * @param now 当前时间点
     * @return 如果稳定电平发生变化且需要上报则返回true，否则返回false
     */
    bool update(int raw, std::chrono::steady_clock::time_point now){
        raw = raw ? 1 : 0;  // 确保输入为0或1
        
        if (raw != last_raw_) {
            // 输入发生变化，记录变化时间和新值
            last_raw_ = raw;
            last_change_ = now;
            return false; // 尚未稳定，不报告变化
        }
        
        // 计算自上次变化以来的时间
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_change_).count();
        
        // 如果输入稳定且与当前稳定值不同，且已超过去抖时间，则更新稳定值
        if (raw != stable_ && ms >= debounce_) {
            stable_ = raw;
            return true;  // 报告稳定电平变化
        }
        return false;
    }
    
    /**
     * @brief 获取当前稳定电平
     * @return 当前稳定的电平值（0或1）
     */
    int stable() const { return stable_; }
    
    /**
     * @brief 设置去抖时间
     * @param ms 新的去抖时间（毫秒）
     */
    void setDebounce(int ms){ debounce_ = ms; }
    
private:
    int debounce_;                                    ///< 去抖时间（毫秒）
    int stable_;                                      ///< 当前稳定的电平值
    int last_raw_;                                    ///< 上次的原始输入值
    std::chrono::steady_clock::time_point last_change_; ///< 上次输入变化的时间点
};
