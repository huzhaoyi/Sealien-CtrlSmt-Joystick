/*
 * @Author: Auto Generated
 * @Date: 2025-01-XX
 * @Description: 调试配置管理器实现
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "utils/debug_config.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <vector>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <cstdlib>

void DebugConfig::fromMap(const std::map<std::string, bool>& config_map) {
    // 辅助函数：将字符串转换为小写
    auto toLower = [](const std::string& s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    };
    
    // 从映射表读取配置
    for (const auto& pair : config_map) {
        std::string key = toLower(pair.first);
        bool value = pair.second;
        
        if (key == "core" || key == "debug_core") {
            debug_core = value;
        } else if (key == "config" || key == "debug_config") {
            debug_config = value;
        } else if (key == "modbus_comm" || key == "debug_modbus_comm") {
            debug_modbus_comm = value;
        } else if (key == "modbus_input_reg" || key == "debug_modbus_input_reg") {
            debug_modbus_input_reg = value;
        } else if (key == "modbus_holding_reg" || key == "debug_modbus_holding_reg") {
            debug_modbus_holding_reg = value;
        } else if (key == "modbus_coil" || key == "debug_modbus_coil") {
            debug_modbus_coil = value;
        } else if (key == "modbus_discrete_input" || key == "debug_modbus_discrete_input") {
            debug_modbus_discrete_input = value;
        } else if (key == "modbus_voltage_conv" || key == "debug_modbus_voltage_conv") {
            debug_modbus_voltage_conv = value;
        } else if (key == "modbus_write" || key == "debug_modbus_write") {
            debug_modbus_write = value;
        } else if (key == "modbus_hex_frame" || key == "debug_modbus_hex_frame") {
            debug_modbus_hex_frame = value;
        } else if (key == "joystick" || key == "debug_joystick") {
            debug_joystick = value;
        } else if (key == "ros2_comm" || key == "debug_ros2_comm") {
            debug_ros2_comm = value;
        } else if (key == "ros2_axes" || key == "debug_ros2_axes") {
            debug_ros2_axes = value;
        } else if (key == "ros2_buttons" || key == "debug_ros2_buttons") {
            debug_ros2_buttons = value;
        } else if (key == "ros2_twist" || key == "debug_ros2_twist") {
            debug_ros2_twist = value;
        } else if (key == "ros2_status" || key == "debug_ros2_status") {
            debug_ros2_status = value;
        } else if (key == "ros2_publish" || key == "debug_ros2_publish") {
            debug_ros2_publish = value;
        } else if (key == "ros2_subscribe" || key == "debug_ros2_subscribe") {
            debug_ros2_subscribe = value;
        } else if (key == "utils" || key == "debug_utils") {
            debug_utils = value;
        } else if (key == "mapping" || key == "debug_mapping") {
            debug_mapping = value;
        } else if (key == "uinput" || key == "debug_uinput") {
            debug_uinput = value;
        } else if (key == "valve_control" || key == "debug_valve_control") {
            debug_valve_control = value;
        } else if (key == "valve_control_status" || key == "debug_valve_control_status") {
            debug_valve_control_status = value;
        } else if (key == "valve_control_status_registers" || key == "debug_valve_control_status_registers") {
            debug_valve_control_status_registers = value;
        } else if (key == "valve_control_status_key" || key == "debug_valve_control_status_key") {
            debug_valve_control_status_key = value;
        } else if (key == "valve_control_status_current" || key == "debug_valve_control_status_current") {
            debug_valve_control_status_current = value;
        } else if (key == "valve_control_status_sensor" || key == "debug_valve_control_status_sensor") {
            debug_valve_control_status_sensor = value;
        } else if (key == "valve_control_status_encoder" || key == "debug_valve_control_status_encoder") {
            debug_valve_control_status_encoder = value;
        } else if (key == "valve_control_status_proximity" || key == "debug_valve_control_status_proximity") {
            debug_valve_control_status_proximity = value;
        } else if (key == "valve_control_status_env" || key == "debug_valve_control_status_env") {
            debug_valve_control_status_env = value;
        } else if (key == "valve_control_status_sensor_status" || key == "debug_valve_control_status_sensor_status") {
            debug_valve_control_status_sensor_status = value;
        } else if (key == "valve_control_status_valve_status" || key == "debug_valve_control_status_valve_status") {
            debug_valve_control_status_valve_status = value;
        } else if (key == "valve_control_write" || key == "debug_valve_control_write") {
            debug_valve_control_write = value;
        } else if (key == "valve_control_joystick" || key == "debug_valve_control_joystick") {
            debug_valve_control_joystick = value;
        }
    }
}

std::map<std::string, bool> DebugConfig::toMap() const {
    std::map<std::string, bool> result;
    result["core"] = debug_core;
    result["config"] = debug_config;
    result["modbus_comm"] = debug_modbus_comm;
    result["modbus_input_reg"] = debug_modbus_input_reg;
    result["modbus_holding_reg"] = debug_modbus_holding_reg;
    result["modbus_coil"] = debug_modbus_coil;
    result["modbus_discrete_input"] = debug_modbus_discrete_input;
    result["modbus_voltage_conv"] = debug_modbus_voltage_conv;
    result["modbus_write"] = debug_modbus_write;
    result["modbus_hex_frame"] = debug_modbus_hex_frame;
    result["joystick"] = debug_joystick;
    result["ros2_comm"] = debug_ros2_comm;
    result["ros2_axes"] = debug_ros2_axes;
    result["ros2_buttons"] = debug_ros2_buttons;
    result["ros2_twist"] = debug_ros2_twist;
    result["ros2_status"] = debug_ros2_status;
    result["ros2_publish"] = debug_ros2_publish;
    result["ros2_subscribe"] = debug_ros2_subscribe;
    result["utils"] = debug_utils;
    result["mapping"] = debug_mapping;
    result["uinput"] = debug_uinput;
    result["valve_control"] = debug_valve_control;
    result["valve_control_status"] = debug_valve_control_status;
    result["valve_control_status_registers"] = debug_valve_control_status_registers;
    result["valve_control_status_key"] = debug_valve_control_status_key;
    result["valve_control_status_current"] = debug_valve_control_status_current;
    result["valve_control_status_sensor"] = debug_valve_control_status_sensor;
    result["valve_control_status_encoder"] = debug_valve_control_status_encoder;
    result["valve_control_status_proximity"] = debug_valve_control_status_proximity;
    result["valve_control_status_env"] = debug_valve_control_status_env;
    result["valve_control_status_sensor_status"] = debug_valve_control_status_sensor_status;
    result["valve_control_status_valve_status"] = debug_valve_control_status_valve_status;
    result["valve_control_write"] = debug_valve_control_write;
    result["valve_control_joystick"] = debug_valve_control_joystick;
    return result;
}

void DebugConfig::setDefaults() {
    debug_core = true;
    debug_config = true;
    debug_modbus_comm = true;
    debug_modbus_input_reg = false;
    debug_modbus_holding_reg = false;
    debug_modbus_coil = false;
    debug_modbus_discrete_input = false;
    debug_modbus_voltage_conv = false;
    debug_modbus_write = false;
    debug_modbus_hex_frame = false;
    debug_joystick = true;
    debug_ros2_comm = true;
    debug_ros2_axes = false;
    debug_ros2_buttons = false;
    debug_ros2_twist = false;
    debug_ros2_status = false;
    debug_ros2_publish = false;
    debug_ros2_subscribe = false;
    debug_utils = true;
    debug_mapping = true;
    debug_uinput = true;
    debug_valve_control = true;
    debug_valve_control_status = false;
    debug_valve_control_status_registers = false;
    debug_valve_control_status_key = false;
    debug_valve_control_status_current = false;
    debug_valve_control_status_sensor = false;
    debug_valve_control_status_encoder = false;
    debug_valve_control_status_proximity = false;
    debug_valve_control_status_env = false;
    debug_valve_control_status_sensor_status = false;
    debug_valve_control_status_valve_status = false;
    debug_valve_control_write = false;
    debug_valve_control_joystick = false;
}

void DebugConfig::enableAll() {
    debug_core = true;
    debug_config = true;
    debug_modbus_comm = true;
    debug_modbus_input_reg = true;
    debug_modbus_holding_reg = true;
    debug_modbus_coil = true;
    debug_modbus_discrete_input = true;
    debug_modbus_voltage_conv = true;
    debug_modbus_write = true;
    debug_modbus_hex_frame = true;
    debug_joystick = true;
    debug_ros2_comm = true;
    debug_ros2_axes = true;
    debug_ros2_buttons = true;
    debug_ros2_twist = true;
    debug_ros2_status = true;
    debug_ros2_publish = true;
    debug_ros2_subscribe = true;
    debug_utils = true;
    debug_mapping = true;
    debug_uinput = true;
    debug_valve_control = true;
    debug_valve_control_status = true;
    debug_valve_control_status_registers = true;
    debug_valve_control_status_key = true;
    debug_valve_control_status_current = true;
    debug_valve_control_status_sensor = true;
    debug_valve_control_status_encoder = true;
    debug_valve_control_status_proximity = true;
    debug_valve_control_status_env = true;
    debug_valve_control_status_sensor_status = true;
    debug_valve_control_status_valve_status = true;
    debug_valve_control_write = true;
    debug_valve_control_joystick = true;
}

void DebugConfig::disableAll() {
    debug_core = false;
    debug_config = false;
    debug_modbus_comm = false;
    debug_modbus_input_reg = false;
    debug_modbus_holding_reg = false;
    debug_modbus_coil = false;
    debug_modbus_discrete_input = false;
    debug_modbus_voltage_conv = false;
    debug_modbus_write = false;
    debug_modbus_hex_frame = false;
    debug_joystick = false;
    debug_ros2_comm = false;
    debug_ros2_axes = false;
    debug_ros2_buttons = false;
    debug_ros2_twist = false;
    debug_ros2_status = false;
    debug_ros2_publish = false;
    debug_ros2_subscribe = false;
    debug_utils = false;
    debug_mapping = false;
    debug_uinput = false;
    debug_valve_control = false;
    debug_valve_control_status = false;
    debug_valve_control_status_registers = false;
    debug_valve_control_status_key = false;
    debug_valve_control_status_current = false;
    debug_valve_control_status_sensor = false;
    debug_valve_control_status_encoder = false;
    debug_valve_control_status_proximity = false;
    debug_valve_control_status_env = false;
    debug_valve_control_status_sensor_status = false;
    debug_valve_control_status_valve_status = false;
    debug_valve_control_write = false;
    debug_valve_control_joystick = false;
}

DebugConfigManager& DebugConfigManager::getInstance() {
    static DebugConfigManager instance;
    return instance;
}

void DebugConfigManager::applyConfig(const DebugConfig& config) {
    config_ = config;
}

bool DebugConfigManager::loadFromFile(const std::string& config_path) {
    try {
        std::string file_path = config_path;
        
        // 如果路径为空，尝试从环境变量或默认路径加载
        if (file_path.empty()) {
            // 首先尝试从环境变量获取路径
            const char* env_path = std::getenv("SEALIEN_DEBUG_CONFIG");
            if (env_path && env_path[0] != '\0') {
                file_path = env_path;
            } else {
                // 使用默认路径：当前目录或config目录
                std::vector<std::string> default_paths = {
                    "config/debug.yaml",
                    "./config/debug.yaml",
                    "../config/debug.yaml",
                    "/opt/sealien-joystick/config/debug.yaml"
                };
                
                // 查找第一个存在的文件
                for (const auto& path : default_paths) {
                    std::ifstream test_file(path);
                    if (test_file.good()) {
                        file_path = path;
                        test_file.close();
                        break;
                    }
                }
            }
        }
        
        // 如果仍然没有找到文件，使用默认配置
        if (file_path.empty()) {
            config_.setDefaults();
            std::cout << "[DEBUG CONFIG] No debug config file found, using default configuration" << std::endl;
            std::cout << "[DEBUG CONFIG] Searched paths: config/debug.yaml, ./config/debug.yaml, ../config/debug.yaml, /opt/sealien-joystick/config/debug.yaml" << std::endl;
            std::cout << "[DEBUG CONFIG] You can set SEALIEN_DEBUG_CONFIG environment variable to specify custom path" << std::endl;
            return true;  // 使用默认配置，不算错误
        }
        
        // 检查文件是否存在
        std::ifstream file(file_path);
        if (!file.good()) {
            // 文件不存在，使用默认配置
            config_.setDefaults();
            std::cout << "[DEBUG CONFIG] Debug config file not found: " << file_path << ", using default configuration" << std::endl;
            return true;  // 使用默认配置，不算错误
        }
        file.close();
        
        // 加载YAML配置文件
        YAML::Node yaml_config = YAML::LoadFile(file_path);
        
        // 解析调试配置
        std::map<std::string, bool> debug_map;
        
        if (yaml_config["core"]) debug_map["core"] = yaml_config["core"].as<bool>();
        if (yaml_config["config"]) debug_map["config"] = yaml_config["config"].as<bool>();
        if (yaml_config["modbus_comm"]) debug_map["modbus_comm"] = yaml_config["modbus_comm"].as<bool>();
        if (yaml_config["modbus_input_reg"]) debug_map["modbus_input_reg"] = yaml_config["modbus_input_reg"].as<bool>();
        if (yaml_config["modbus_holding_reg"]) debug_map["modbus_holding_reg"] = yaml_config["modbus_holding_reg"].as<bool>();
        if (yaml_config["modbus_coil"]) debug_map["modbus_coil"] = yaml_config["modbus_coil"].as<bool>();
        if (yaml_config["modbus_discrete_input"]) debug_map["modbus_discrete_input"] = yaml_config["modbus_discrete_input"].as<bool>();
        if (yaml_config["modbus_voltage_conv"]) debug_map["modbus_voltage_conv"] = yaml_config["modbus_voltage_conv"].as<bool>();
        if (yaml_config["modbus_write"]) debug_map["modbus_write"] = yaml_config["modbus_write"].as<bool>();
        if (yaml_config["modbus_hex_frame"]) debug_map["modbus_hex_frame"] = yaml_config["modbus_hex_frame"].as<bool>();
        if (yaml_config["joystick"]) debug_map["joystick"] = yaml_config["joystick"].as<bool>();
        if (yaml_config["ros2_comm"]) debug_map["ros2_comm"] = yaml_config["ros2_comm"].as<bool>();
        if (yaml_config["ros2_axes"]) debug_map["ros2_axes"] = yaml_config["ros2_axes"].as<bool>();
        if (yaml_config["ros2_buttons"]) debug_map["ros2_buttons"] = yaml_config["ros2_buttons"].as<bool>();
        if (yaml_config["ros2_twist"]) debug_map["ros2_twist"] = yaml_config["ros2_twist"].as<bool>();
        if (yaml_config["ros2_status"]) debug_map["ros2_status"] = yaml_config["ros2_status"].as<bool>();
        if (yaml_config["ros2_publish"]) debug_map["ros2_publish"] = yaml_config["ros2_publish"].as<bool>();
        if (yaml_config["ros2_subscribe"]) debug_map["ros2_subscribe"] = yaml_config["ros2_subscribe"].as<bool>();
        if (yaml_config["utils"]) debug_map["utils"] = yaml_config["utils"].as<bool>();
        if (yaml_config["mapping"]) debug_map["mapping"] = yaml_config["mapping"].as<bool>();
        if (yaml_config["uinput"]) debug_map["uinput"] = yaml_config["uinput"].as<bool>();
        if (yaml_config["valve_control"]) debug_map["valve_control"] = yaml_config["valve_control"].as<bool>();
        if (yaml_config["valve_control_status"]) debug_map["valve_control_status"] = yaml_config["valve_control_status"].as<bool>();
        if (yaml_config["valve_control_status_registers"]) debug_map["valve_control_status_registers"] = yaml_config["valve_control_status_registers"].as<bool>();
        if (yaml_config["valve_control_status_key"]) debug_map["valve_control_status_key"] = yaml_config["valve_control_status_key"].as<bool>();
        if (yaml_config["valve_control_status_current"]) debug_map["valve_control_status_current"] = yaml_config["valve_control_status_current"].as<bool>();
        if (yaml_config["valve_control_status_sensor"]) debug_map["valve_control_status_sensor"] = yaml_config["valve_control_status_sensor"].as<bool>();
        if (yaml_config["valve_control_status_encoder"]) debug_map["valve_control_status_encoder"] = yaml_config["valve_control_status_encoder"].as<bool>();
        if (yaml_config["valve_control_status_proximity"]) debug_map["valve_control_status_proximity"] = yaml_config["valve_control_status_proximity"].as<bool>();
        if (yaml_config["valve_control_status_env"]) debug_map["valve_control_status_env"] = yaml_config["valve_control_status_env"].as<bool>();
        if (yaml_config["valve_control_status_sensor_status"]) debug_map["valve_control_status_sensor_status"] = yaml_config["valve_control_status_sensor_status"].as<bool>();
        if (yaml_config["valve_control_status_valve_status"]) debug_map["valve_control_status_valve_status"] = yaml_config["valve_control_status_valve_status"].as<bool>();
        if (yaml_config["valve_control_write"]) debug_map["valve_control_write"] = yaml_config["valve_control_write"].as<bool>();
        if (yaml_config["valve_control_joystick"]) debug_map["valve_control_joystick"] = yaml_config["valve_control_joystick"].as<bool>();
        
        // 先设置默认值，然后应用从文件读取的配置
        config_.setDefaults();
        config_.fromMap(debug_map);
        
        // 注意：这里不能使用DEBUG宏，因为可能还在初始化阶段
        // 使用标准输出提示配置文件已加载
        std::cout << "[DEBUG CONFIG] Loaded debug configuration from: " << file_path << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        // 加载失败，使用默认配置
        config_.setDefaults();
        std::cout << "[DEBUG CONFIG] Failed to load debug config file, using defaults: " << e.what() << std::endl;
        return false;
    }
}

