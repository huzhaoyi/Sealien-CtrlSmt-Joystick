/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-09-08 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-09-08 11:29:37
 * @FilePath: src/core/main.cpp
 * @Description: Modbus摇杆主程序
 * 该程序通过Modbus协议读取工业设备数据，并将其映射为虚拟摇杆输入
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "application_controller.h"
#include "constants.h"
#include "utils.h"
#include "utils/debug_config.h"

#include <iostream>
#include <string>
#include <algorithm>

/**
 * @brief 显示使用帮助信息
 */
static void showUsage(const char* program_name) {
    std::cout << "用法: " << program_name << " [选项] [配置文件]\n";
    std::cout << "\n";
    std::cout << "参数:\n";
    std::cout << "  配置文件      配置文件路径 (可选，默认: USB 模式)\n";
    std::cout << "\n";
    std::cout << "选项:\n";
    std::cout << "  -h, --help         显示此帮助信息\n";
    std::cout << "\n";
    std::cout << "功能:\n";
    std::cout << "  - 默认模式: 仅 USB 摇杆 (无需配置文件)\n";
    std::cout << "  - Modbus 模式: 通过指定包含 'enable_modbus: true' 的配置文件启用\n";
    std::cout << "  - 自动检测 USB 摇杆和 Modbus 设备\n";
    std::cout << "  - USB 和 Modbus 模式之间动态切换\n";
    std::cout << "  - 可用时自动集成 ROS2\n";
    std::cout << "\n";
    std::cout << "示例:\n";
    std::cout << "  " << program_name << "                           # USB 模式 (无需配置)\n";
    std::cout << "  " << program_name << " config/modbus/AMSAMOTION_IO8R.yaml # Modbus 模式配置\n";
}


/**
 * @brief 主函数 - Modbus摇杆程序入口点
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 程序退出码
 */
int main(int argc, char** argv) {
    // 首先加载调试配置文件（必须在任何DEBUG宏调用之前）
    // 这样后续的DEBUG日志才能根据配置文件正确输出
    DebugConfigManager::getInstance().loadFromFile();
    
    // 解析命令行参数
    std::string config_path = "";  // 默认为空，表示USB模式
    bool use_config = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            showUsage(argv[0]);
            return 0;
        } else if (arg.find("--") != 0) {
            // 不是选项参数，认为是配置文件路径
            config_path = arg;
            use_config = true;
        }
    }

    DEBUG_CORE_LOG("正在启动 Sealien 摇杆应用程序...");
    if (use_config) {
        DEBUG_CORE_LOG("配置文件: " << config_path);
    } else {
        DEBUG_CORE_LOG("未指定配置文件，以 USB 模式运行");
    }

    try {
        // 创建应用程序控制器
        ApplicationController app_controller;

        // 初始化应用程序
        if (!app_controller.initialize(config_path, true)) {  // 总是启用ROS2
            DEBUG_CORE_LOG("应用程序控制器初始化失败");
            return 1;
        }
        
        DEBUG_CORE_LOG("应用程序初始化成功");
        DEBUG_CORE_LOG("运行模式: " << app_controller.getRunMode());

        // 运行应用程序主循环
        if (!app_controller.run()) {
            DEBUG_CORE_LOG("应用程序运行失败");
            return 1;
        }

        DEBUG_CORE_LOG("应用程序成功完成");
        return 0;

    } catch (const std::exception& e) {
        DEBUG_CORE_LOG("应用程序异常失败: " << e.what());
        return 1;
    }
}
