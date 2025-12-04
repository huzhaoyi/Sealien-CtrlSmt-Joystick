/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:28:43
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:28:43
 * @FilePath: include/keycodes.h
 * @Description: Linux输入事件代码映射头文件
 * 定义轴和按键名称与Linux内核输入事件代码之间的映射关系
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */
#pragma once
#include <linux/input.h>
#include <map>
#include <string>

/**
 * @brief 轴名称到代码的映射表
 * 
 * 该映射表将人类可读的轴名称映射到Linux内核定义的轴代码。
 * 支持常见的游戏控制器轴，如X/Y/Z轴、旋转轴、油门、方向舵等。
 * 
 * 主要轴类型：
 * - ABS_X, ABS_Y, ABS_Z: 主要3D轴
 * - ABS_RX, ABS_RY, ABS_RZ: 旋转轴
 * - ABS_THROTTLE, ABS_RUDDER: 飞行控制器轴
 * - ABS_WHEEL, ABS_GAS, ABS_BRAKE: 车辆控制器轴
 * - ABS_TILT_X, ABS_TILT_Y: 倾斜轴
 * - ABS_DISTANCE, ABS_PRESSURE: 距离和压力轴
 * - ABS_MISC: 杂项轴
 */
static inline const std::map<std::string,int> kAbsNameToCode = {
    {"ABS_X",ABS_X},           ///< X轴（左右）
    {"ABS_Y",ABS_Y},           ///< Y轴（前后）
    {"ABS_Z",ABS_Z},           ///< Z轴（上下）
    {"ABS_RX",ABS_RX},         ///< X旋转轴
    {"ABS_RY",ABS_RY},         ///< Y旋转轴
    {"ABS_RZ",ABS_RZ},         ///< Z旋转轴
    {"ABS_THROTTLE",ABS_THROTTLE}, ///< 油门轴
    {"ABS_RUDDER",ABS_RUDDER},     ///< 方向舵轴
    {"ABS_WHEEL",ABS_WHEEL},       ///< 方向盘轴
    {"ABS_GAS",ABS_GAS},           ///< 油门踏板
    {"ABS_BRAKE",ABS_BRAKE},       ///< 刹车踏板
    {"ABS_TILT_X",ABS_TILT_X},     ///< X倾斜轴
    {"ABS_TILT_Y",ABS_TILT_Y},     ///< Y倾斜轴
    {"ABS_DISTANCE",ABS_DISTANCE}, ///< 距离轴
    {"ABS_PRESSURE",ABS_PRESSURE}, ///< 压力轴
    {"ABS_MISC",ABS_MISC}          ///< 杂项轴
};

/**
 * @brief 按键名称到代码的映射表
 * 
 * 该映射表将人类可读的按键名称映射到Linux内核定义的按键代码。
 * 支持常见的游戏控制器按键，包括动作键、肩键、功能键等。
 * 
 * 按键类型说明：
 * - BTN_A, BTN_B, BTN_X, BTN_Y: 主要动作键
 * - BTN_SOUTH, BTN_EAST, BTN_NORTH, BTN_WEST: 方向动作键（与A/B/X/Y对应）
 * - BTN_TL, BTN_TR: 左/右肩键
 * - BTN_TL2, BTN_TR2: 左/右扳机键
 * - BTN_SELECT, BTN_START, BTN_MODE: 功能键
 * 
 * 注意：BTN_TRIGGER_HAPPY系列按键（BTN_TRIGGER_HAPPY1到BTN_TRIGGER_HAPPY96）
 * 在mapping.cpp中特殊处理，不在此映射表中。
 */
static inline const std::map<std::string,int> kBtnNameToCode = {
    {"BTN_A",BTN_A},           ///< A键
    {"BTN_B",BTN_B},           ///< B键
    {"BTN_X",BTN_X},           ///< X键
    {"BTN_Y",BTN_Y},           ///< Y键
    {"BTN_SOUTH",BTN_A},       ///< 南键（对应A键）
    {"BTN_EAST",BTN_B},        ///< 东键（对应B键）
    {"BTN_NORTH",BTN_X},       ///< 北键（对应X键）
    {"BTN_WEST",BTN_Y},        ///< 西键（对应Y键）
    {"BTN_TL",BTN_TL},         ///< 左肩键
    {"BTN_TR",BTN_TR},         ///< 右肩键
    {"BTN_TL2",BTN_TL2},       ///< 左扳机键
    {"BTN_TR2",BTN_TR2},       ///< 右扳机键
    {"BTN_SELECT",BTN_SELECT}, ///< 选择键
    {"BTN_START",BTN_START},   ///< 开始键
    {"BTN_MODE",BTN_MODE}      ///< 模式键
};
