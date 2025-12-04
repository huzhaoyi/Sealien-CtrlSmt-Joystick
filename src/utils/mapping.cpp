/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:29:46
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:29:46
 * @FilePath: src/mapping.cpp
 * @Description: 按键和轴代码映射模块
 * 提供字符串与Linux输入事件代码之间的双向转换功能
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "mapping.h"
#include "input/keycodes.h"
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <algorithm>

/**
 * @brief 去除字符串首尾的空白字符
 * @param s 待处理的字符串
 * @return 去除空白字符后的字符串
 */
static inline std::string trim(const std::string& s){
    auto a = s.begin(); 
    while (a!=s.end() && std::isspace((unsigned char)*a)) ++a;
    
    auto b = s.end();   
    do { --b; } while (b!=a && std::isspace((unsigned char)*b));
    
    return std::string(a, b+1);
}

/**
 * @brief 将轴名称字符串转换为对应的轴代码
 * @param name 轴名称（如"ABS_X", "ABS_Y"等）
 * @return 对应的轴代码
 * @throws std::runtime_error 当轴名称未知时
 */
int StrToAbsCode(const std::string& name, const std::string& contextPath) {
    auto it = kAbsNameToCode.find(trim(name));
    if (it != kAbsNameToCode.end()) return it->second;
    if (!contextPath.empty())
        throw std::runtime_error("Invalid axis code at '" + contextPath + "': '" + name + "'");
    throw std::runtime_error("Unknown ABS code: " + name);
}

/**
 * @brief 将按键名称字符串转换为对应的按键代码
 * @param name 按键名称（如"BTN_1", "BTN_TRIGGER_HAPPY1"等）
 * @return 对应的按键代码
 * @throws std::runtime_error 当按键名称未知或索引超出范围时
 */
int StrToBtnCode(const std::string& name, const std::string& contextPath) {
    std::string s = trim(name);
    
    // 特殊处理BTN_TRIGGER_HAPPY系列按键
    if (s.rfind("BTN_TRIGGER_HAPPY",0)==0) {
        int idx = std::stoi(s.substr(17));  // 提取索引号
        if (idx < 1 || idx > 96) throw std::runtime_error("BTN_TRIGGER_HAPPY index out of range: "+std::to_string(idx));
        return BTN_TRIGGER_HAPPY1 + (idx - 1);  // 计算实际代码值
    }
    
    // 查找标准按键代码
    auto it = kBtnNameToCode.find(s);
    if (it != kBtnNameToCode.end()) return it->second;
    if (!contextPath.empty())
        throw std::runtime_error("Invalid button code at '" + contextPath + "': '" + name + "'");
    throw std::runtime_error("Unknown BTN code: " + name);
}

/**
 * @brief 将轴代码转换为对应的轴名称字符串
 * @param code 轴代码
 * @return 对应的轴名称，如果未找到则返回"ABS_UNKNOWN"
 */
std::string AbsCodeToString(int code){
    // 遍历映射表查找匹配的代码
    for (auto& kv : kAbsNameToCode) 
        if (kv.second==code) return kv.first;
    return "ABS_UNKNOWN";
}

/**
 * @brief 将按键代码转换为对应的按键名称字符串
 * @param code 按键代码
 * @return 对应的按键名称，如果未找到则返回"BTN_UNKNOWN"
 */
std::string BtnCodeToString(int code){
    // 特殊处理BTN_TRIGGER_HAPPY系列按键
    if (code>=BTN_TRIGGER_HAPPY1 && code<=BTN_TRIGGER_HAPPY1+95) {
        int idx = code - BTN_TRIGGER_HAPPY1 + 1;
        return std::string("BTN_TRIGGER_HAPPY") + std::to_string(idx);
    }
    
    // 遍历映射表查找匹配的代码
    for (auto& kv : kBtnNameToCode) 
        if (kv.second==code) return kv.first;
    return "BTN_UNKNOWN";
}
