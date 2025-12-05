/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-09-02 15:30:00
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-09-02 15:30:00
 * @FilePath: src/utils/joystick_mapping.cpp
 * @Description: 手柄映射功能实现文件
 * 实现手柄映射管理器和处理器的功能
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "joystick_mapping.h"
#include "../../include/utils/utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <yaml-cpp/yaml.h>

namespace sealien_joystick {

// ============================================================================
// 静态常量定义
// ============================================================================

// 手柄厂商ID映射
static const std::map<uint16_t, JoystickVendor> VENDOR_ID_MAP = {
    {0x045E, JoystickVendor::XBOX},           // Microsoft
    {0x054C, JoystickVendor::PLAYSTATION},    // Sony
    {0x046D, JoystickVendor::LOGITECH},       // Logitech
    {0x044F, JoystickVendor::THRUSTMASTER},   // Thrustmaster
    {0x06A3, JoystickVendor::SAITEK},         // Saitek
};

// 手柄产品ID映射
static const std::map<std::pair<uint16_t, uint16_t>, JoystickProduct> PRODUCT_ID_MAP = {
    // Xbox手柄
    {{0x045E, 0x028E}, JoystickProduct::XBOX_360_CONTROLLER},
    {{0x045E, 0x02D1}, JoystickProduct::XBOX_ONE_CONTROLLER},
    {{0x045E, 0x0B12}, JoystickProduct::XBOX_SERIES_CONTROLLER},
    
    // PlayStation手柄
    {{0x054C, 0x0268}, JoystickProduct::PS3_CONTROLLER},
    {{0x054C, 0x05C4}, JoystickProduct::PS4_CONTROLLER},
    {{0x054C, 0x0CE6}, JoystickProduct::PS5_CONTROLLER},
    
    // 罗技手柄
    {{0x046D, 0xC21D}, JoystickProduct::LOGITECH_F310},
    {{0x046D, 0xC21F}, JoystickProduct::LOGITECH_F710},
    {{0x046D, 0xC21E}, JoystickProduct::LOGITECH_GAMEPAD_F310},
    
    // Thrustmaster手柄
    {{0x044F, 0xB10A}, JoystickProduct::THRUSTMASTER_T16000M},
};

// ============================================================================
// JoystickMappingManager 实现
// ============================================================================

JoystickMappingManager::JoystickMappingManager() {
    initializeDefaultMappings();
}

JoystickMappingManager::~JoystickMappingManager() = default;

bool JoystickMappingManager::loadMappingConfig(const std::string& config_file) {
    try {
        YAML::Node config = YAML::LoadFile(config_file);
        
        if (!config["joystick_mappings"]) {
            DEBUG_MAPPING_ERROR("配置文件格式错误：缺少 joystick_mappings 节点");
            return false;
        }
        
        auto mappings = config["joystick_mappings"];
        for (const auto& mapping : mappings) {
            JoystickMappingConfig mapping_config;
            
            // 读取设备信息
            mapping_config.vendor_id = mapping["vendor_id"].as<uint16_t>();
            mapping_config.product_id = mapping["product_id"].as<uint16_t>();
            mapping_config.device_name = mapping["device_name"].as<std::string>();
            mapping_config.config_name = mapping["config_name"].as<std::string>();
            
            // 读取全局配置
            mapping_config.default_deadzone = mapping["default_deadzone"].as<double>();
            mapping_config.default_scale_factor = mapping["default_scale_factor"].as<double>();
            mapping_config.enable_auto_detection = mapping["enable_auto_detection"].as<bool>();
            
            // 读取轴映射配置
            if (mapping["axis_mappings"]) {
                for (const auto& axis : mapping["axis_mappings"]) {
                    AxisMappingConfig axis_config;
                    axis_config.physical_axis_index = axis["physical_axis_index"].as<int>();
                    axis_config.mapping_type = static_cast<AxisMappingType>(axis["mapping_type"].as<int>());
                    axis_config.custom_name = axis["custom_name"].as<std::string>();
                    axis_config.invert = axis["invert"].as<bool>();
                    axis_config.deadzone = axis["deadzone"].as<double>();
                    axis_config.scale_factor = axis["scale_factor"].as<double>();
                    axis_config.min_value = axis["min_value"].as<double>();
                    axis_config.max_value = axis["max_value"].as<double>();
                    axis_config.enabled = axis["enabled"].as<bool>();
                    mapping_config.axis_mappings.push_back(axis_config);
                }
            }
            
            // 读取按钮映射配置
            if (mapping["button_mappings"]) {
                for (const auto& button : mapping["button_mappings"]) {
                    ButtonMappingConfig button_config;
                    button_config.physical_button_index = button["physical_button_index"].as<int>();
                    button_config.function_type = static_cast<ButtonFunctionType>(button["function_type"].as<int>());
                    button_config.custom_name = button["custom_name"].as<std::string>();
                    button_config.active_high = button["active_high"].as<bool>();
                    button_config.debounce_ms = button["debounce_ms"].as<int>();
                    button_config.enabled = button["enabled"].as<bool>();
                    mapping_config.button_mappings.push_back(button_config);
                }
            }
            
            addOrUpdateMappingConfig(mapping_config);
        }
        
        config_file_path_ = config_file;
        return true;
        
    } catch (const std::exception& e) {
        DEBUG_MAPPING_ERROR("加载映射配置文件失败：" << e.what());
        return false;
    }
}

bool JoystickMappingManager::saveMappingConfig(const std::string& config_file) {
    try {
        YAML::Node root;
        YAML::Node mappings;
        
        for (const auto& [key, config] : id_mappings_) {
            YAML::Node mapping;
            
            // 保存设备信息
            mapping["vendor_id"] = config->vendor_id;
            mapping["product_id"] = config->product_id;
            mapping["device_name"] = config->device_name;
            mapping["config_name"] = config->config_name;
            
            // 保存全局配置
            mapping["default_deadzone"] = config->default_deadzone;
            mapping["default_scale_factor"] = config->default_scale_factor;
            mapping["enable_auto_detection"] = config->enable_auto_detection;
            
            // 保存轴映射配置
            YAML::Node axis_mappings;
            for (const auto& axis : config->axis_mappings) {
                YAML::Node axis_node;
                axis_node["physical_axis_index"] = axis.physical_axis_index;
                axis_node["mapping_type"] = static_cast<int>(axis.mapping_type);
                axis_node["custom_name"] = axis.custom_name;
                axis_node["invert"] = axis.invert;
                axis_node["deadzone"] = axis.deadzone;
                axis_node["scale_factor"] = axis.scale_factor;
                axis_node["min_value"] = axis.min_value;
                axis_node["max_value"] = axis.max_value;
                axis_node["enabled"] = axis.enabled;
                axis_mappings.push_back(axis_node);
            }
            mapping["axis_mappings"] = axis_mappings;
            
            // 保存按钮映射配置
            YAML::Node button_mappings;
            for (const auto& button : config->button_mappings) {
                YAML::Node button_node;
                button_node["physical_button_index"] = button.physical_button_index;
                button_node["function_type"] = static_cast<int>(button.function_type);
                button_node["custom_name"] = button.custom_name;
                button_node["active_high"] = button.active_high;
                button_node["debounce_ms"] = button.debounce_ms;
                button_node["enabled"] = button.enabled;
                button_mappings.push_back(button_node);
            }
            mapping["button_mappings"] = button_mappings;
            
            mappings.push_back(mapping);
        }
        
        root["joystick_mappings"] = mappings;
        
        std::ofstream file(config_file);
        file << root;
        
        return true;
        
    } catch (const std::exception& e) {
        DEBUG_MAPPING_ERROR("保存映射配置文件失败：" << e.what());
        return false;
    }
}

bool JoystickMappingManager::autoDetectAndMap(const JoystickDeviceInfo& device_info) {
    // 检查是否已有映射配置
    auto existing_config = getMappingConfig(device_info.vendor_id, device_info.product_id);
    if (existing_config) {
        return true; // 已有配置，无需重新检测
    }
    
    // 创建默认映射配置
    auto default_config = createDefaultMapping(device_info);
    return addOrUpdateMappingConfig(default_config);
}

std::shared_ptr<JoystickMappingConfig> JoystickMappingManager::getMappingConfig(uint16_t vendor_id, uint16_t product_id) {
    auto key = std::make_pair(vendor_id, product_id);
    auto it = id_mappings_.find(key);
    if (it != id_mappings_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<JoystickMappingConfig> JoystickMappingManager::getMappingConfig(const std::string& device_name) {
    auto it = device_mappings_.find(device_name);
    if (it != device_mappings_.end()) {
        return it->second;
    }
    return nullptr;
}

bool JoystickMappingManager::addOrUpdateMappingConfig(const JoystickMappingConfig& config) {
    auto key = std::make_pair(config.vendor_id, config.product_id);
    auto config_ptr = std::make_shared<JoystickMappingConfig>(config);
    
    id_mappings_[key] = config_ptr;
    device_mappings_[config.device_name] = config_ptr;
    
    return true;
}

bool JoystickMappingManager::removeMappingConfig(uint16_t vendor_id, uint16_t product_id) {
    auto key = std::make_pair(vendor_id, product_id);
    auto it = id_mappings_.find(key);
    if (it != id_mappings_.end()) {
        auto config = it->second;
        device_mappings_.erase(config->device_name);
        id_mappings_.erase(it);
        DEBUG_MAPPING_LOG("已移除映射配置: vendor=" << vendor_id << " product=" << product_id);
        return true;
    }
    DEBUG_MAPPING_LOG("未找到映射配置: vendor=" << vendor_id << " product=" << product_id);
    return false;
}

std::vector<std::shared_ptr<JoystickMappingConfig>> JoystickMappingManager::getAllMappingConfigs() const {
    std::vector<std::shared_ptr<JoystickMappingConfig>> configs;
    for (const auto& [key, config] : id_mappings_) {
        configs.push_back(config);
    }
    return configs;
}

JoystickMappingConfig JoystickMappingManager::createDefaultMapping(const JoystickDeviceInfo& device_info) {
    JoystickMappingConfig config;
    config.vendor_id = device_info.vendor_id;
    config.product_id = device_info.product_id;
    config.device_name = device_info.device_name;
    config.config_name = "Auto_" + device_info.device_name;
    config.default_deadzone = USB_JOYSTICK_DEFAULT_DEADZONE;
    config.default_scale_factor = 1.0;
    config.enable_auto_detection = true;
    
    // 根据手柄类型创建默认映射
    switch (device_info.vendor) {
        case JoystickVendor::XBOX:
            return createXboxDefaultMapping();
        case JoystickVendor::PLAYSTATION:
            return createPlayStationDefaultMapping();
        case JoystickVendor::LOGITECH:
            return createLogitechDefaultMapping();
        default:
            return createGenericDefaultMapping();
    }
}

bool JoystickMappingManager::validateMappingConfig(const JoystickMappingConfig& config) {
    // 检查轴映射的有效性
    for (const auto& axis : config.axis_mappings) {
        if (axis.physical_axis_index < 0 || axis.physical_axis_index >= MAX_JOYSTICK_AXES) {
            DEBUG_MAPPING_ERROR("无效的轴索引: " << axis.physical_axis_index);
            return false;
        }
        if (axis.deadzone < 0.0 || axis.deadzone > 0.5) {
            DEBUG_MAPPING_ERROR("无效的死区值: " << axis.deadzone);
            return false;
        }
        if (axis.scale_factor <= 0.0) {
            DEBUG_MAPPING_ERROR("无效的缩放因子: " << axis.scale_factor);
            return false;
        }
    }
    
    // 检查按钮映射的有效性
    for (const auto& button : config.button_mappings) {
        if (button.physical_button_index < 0 || button.physical_button_index >= MAX_JOYSTICK_BUTTONS) {
            DEBUG_MAPPING_ERROR("无效的按钮索引: " << button.physical_button_index);
            return false;
        }
        if (button.debounce_ms < 0 || button.debounce_ms > 1000) {
            DEBUG_MAPPING_ERROR("无效的防抖值: " << button.debounce_ms);
            return false;
        }
    }
    
    return true;
}

JoystickVendor JoystickMappingManager::getVendorType(uint16_t vendor_id) {
    auto it = VENDOR_ID_MAP.find(vendor_id);
    if (it != VENDOR_ID_MAP.end()) {
        return it->second;
    }
    return JoystickVendor::UNKNOWN;
}

JoystickProduct JoystickMappingManager::getProductType(uint16_t vendor_id, uint16_t product_id) {
    auto key = std::make_pair(vendor_id, product_id);
    auto it = PRODUCT_ID_MAP.find(key);
    if (it != PRODUCT_ID_MAP.end()) {
        return it->second;
    }
    return JoystickProduct::UNKNOWN;
}

std::string JoystickMappingManager::getVendorName(JoystickVendor vendor) {
    switch (vendor) {
        case JoystickVendor::XBOX: return "Xbox";
        case JoystickVendor::PLAYSTATION: return "PlayStation";
        case JoystickVendor::LOGITECH: return "Logitech";
        case JoystickVendor::THRUSTMASTER: return "Thrustmaster";
        case JoystickVendor::SAITEK: return "Saitek";
        case JoystickVendor::GENERIC: return "Generic";
        default: return "Unknown";
    }
}

std::string JoystickMappingManager::getProductName(JoystickProduct product) {
    switch (product) {
        case JoystickProduct::XBOX_360_CONTROLLER: return "Xbox 360 Controller";
        case JoystickProduct::XBOX_ONE_CONTROLLER: return "Xbox One Controller";
        case JoystickProduct::XBOX_SERIES_CONTROLLER: return "Xbox Series Controller";
        case JoystickProduct::PS3_CONTROLLER: return "PS3 Controller";
        case JoystickProduct::PS4_CONTROLLER: return "PS4 Controller";
        case JoystickProduct::PS5_CONTROLLER: return "PS5 Controller";
        case JoystickProduct::LOGITECH_F310: return "Logitech F310";
        case JoystickProduct::LOGITECH_F710: return "Logitech F710";
        case JoystickProduct::LOGITECH_GAMEPAD_F310: return "Logitech Gamepad F310";
        case JoystickProduct::THRUSTMASTER_T16000M: return "Thrustmaster T16000M";
        case JoystickProduct::GENERIC_GAMEPAD: return "Generic Gamepad";
        default: return "Unknown";
    }
}

void JoystickMappingManager::initializeDefaultMappings() {
    // 这里可以预加载一些常用的默认映射配置
}

JoystickMappingConfig JoystickMappingManager::createXboxDefaultMapping() {
    JoystickMappingConfig config;
    config.vendor_id = 0x045E;
    config.product_id = 0x02D1;
    config.device_name = "Xbox One Controller";
    config.config_name = "Xbox_Default";
    config.default_deadzone = 0.12;
    config.default_scale_factor = 1.0;
    config.enable_auto_detection = true;
    
    // Xbox手柄轴映射（左摇杆X/Y，右摇杆X/Y，触发器L/R）
    config.axis_mappings = {
        {0, AxisMappingType::LINEAR_X, "", false, 0.12, 1.0, -1.0, 1.0, true},   // 左摇杆X
        {1, AxisMappingType::LINEAR_Y, "", true, 0.12, 1.0, -1.0, 1.0, true},   // 左摇杆Y
        {2, AxisMappingType::CUSTOM, "left_trigger", false, 0.05, 1.0, -1.0, 1.0, true}, // 左触发器
        {3, AxisMappingType::ANGULAR_Z, "", false, 0.12, 1.0, -1.0, 1.0, true}, // 右摇杆X
        {4, AxisMappingType::ANGULAR_Y, "", true, 0.12, 1.0, -1.0, 1.0, true},  // 右摇杆Y
        {5, AxisMappingType::CUSTOM, "right_trigger", false, 0.05, 1.0, -1.0, 1.0, true}, // 右触发器
    };
    
    // Xbox手柄按钮映射
    config.button_mappings = {
        {0, ButtonFunctionType::EMERGENCY_STOP, "", true, 10, true},   // A按钮
        {1, ButtonFunctionType::MODE_SWITCH, "", true, 10, true},     // B按钮
        {2, ButtonFunctionType::SPEED_BOOST, "", true, 10, true},     // X按钮
        {3, ButtonFunctionType::SPEED_SLOW, "", true, 10, true},      // Y按钮
    };
    
    return config;
}

JoystickMappingConfig JoystickMappingManager::createPlayStationDefaultMapping() {
    JoystickMappingConfig config;
    config.vendor_id = 0x054C;
    config.product_id = 0x05C4;
    config.device_name = "PS4 Controller";
    config.config_name = "PS4_Default";
    config.default_deadzone = 0.12;
    config.default_scale_factor = 1.0;
    config.enable_auto_detection = true;
    
    // PS4手柄轴映射
    config.axis_mappings = {
        {0, AxisMappingType::LINEAR_X, "", false, 0.12, 1.0, -1.0, 1.0, true},   // 左摇杆X
        {1, AxisMappingType::LINEAR_Y, "", true, 0.12, 1.0, -1.0, 1.0, true},     // 左摇杆Y
        {2, AxisMappingType::CUSTOM, "left_trigger", false, 0.05, 1.0, -1.0, 1.0, true}, // L2
        {3, AxisMappingType::ANGULAR_Z, "", false, 0.12, 1.0, -1.0, 1.0, true},   // 右摇杆X
        {4, AxisMappingType::ANGULAR_Y, "", true, 0.12, 1.0, -1.0, 1.0, true},     // 右摇杆Y
        {5, AxisMappingType::CUSTOM, "right_trigger", false, 0.05, 1.0, -1.0, 1.0, true}, // R2
    };
    
    // PS4手柄按钮映射
    config.button_mappings = {
        {0, ButtonFunctionType::EMERGENCY_STOP, "", true, 10, true},   // X按钮
        {1, ButtonFunctionType::MODE_SWITCH, "", true, 10, true},     // 圆形按钮
        {2, ButtonFunctionType::SPEED_BOOST, "", true, 10, true},     // 方形按钮
        {3, ButtonFunctionType::SPEED_SLOW, "", true, 10, true},      // 三角形按钮
    };
    
    return config;
}

JoystickMappingConfig JoystickMappingManager::createLogitechDefaultMapping() {
    JoystickMappingConfig config;
    config.vendor_id = 0x046D;
    config.product_id = 0xC21D;
    config.device_name = "Logitech F310";
    config.config_name = "Logitech_Default";
    config.default_deadzone = 0.12;
    config.default_scale_factor = 1.0;
    config.enable_auto_detection = true;
    
    // 罗技手柄轴映射
    config.axis_mappings = {
        {0, AxisMappingType::LINEAR_X, "", false, 0.12, 1.0, -1.0, 1.0, true},   // 左摇杆X
        {1, AxisMappingType::LINEAR_Y, "", true, 0.12, 1.0, -1.0, 1.0, true},    // 左摇杆Y
        {2, AxisMappingType::CUSTOM, "left_trigger", false, 0.05, 1.0, -1.0, 1.0, true}, // 左触发器
        {3, AxisMappingType::ANGULAR_Z, "", false, 0.12, 1.0, -1.0, 1.0, true},  // 右摇杆X
        {4, AxisMappingType::ANGULAR_Y, "", true, 0.12, 1.0, -1.0, 1.0, true},   // 右摇杆Y
        {5, AxisMappingType::CUSTOM, "right_trigger", false, 0.05, 1.0, -1.0, 1.0, true}, // 右触发器
    };
    
    // 罗技手柄按钮映射
    config.button_mappings = {
        {0, ButtonFunctionType::EMERGENCY_STOP, "", true, 10, true},   // A按钮
        {1, ButtonFunctionType::MODE_SWITCH, "", true, 10, true},      // B按钮
        {2, ButtonFunctionType::SPEED_BOOST, "", true, 10, true},      // X按钮
        {3, ButtonFunctionType::SPEED_SLOW, "", true, 10, true},       // Y按钮
    };
    
    return config;
}

JoystickMappingConfig JoystickMappingManager::createGenericDefaultMapping() {
    JoystickMappingConfig config;
    config.vendor_id = 0x0000;
    config.product_id = 0x0000;
    config.device_name = "Generic Gamepad";
    config.config_name = "Generic_Default";
    config.default_deadzone = 0.12;
    config.default_scale_factor = 1.0;
    config.enable_auto_detection = true;
    
    // 通用手柄轴映射（假设前4个轴为左右摇杆）
    config.axis_mappings = {
        {0, AxisMappingType::LINEAR_X, "", false, 0.12, 1.0, -1.0, 1.0, true},   // 轴0
        {1, AxisMappingType::LINEAR_Y, "", true, 0.12, 1.0, -1.0, 1.0, true},     // 轴1
        {2, AxisMappingType::ANGULAR_Z, "", false, 0.12, 1.0, -1.0, 1.0, true},  // 轴2
        {3, AxisMappingType::ANGULAR_Y, "", true, 0.12, 1.0, -1.0, 1.0, true},   // 轴3
    };
    
    // 通用手柄按钮映射（前4个按钮）
    config.button_mappings = {
        {0, ButtonFunctionType::EMERGENCY_STOP, "", true, 10, true},   // 按钮0
        {1, ButtonFunctionType::MODE_SWITCH, "", true, 10, true},       // 按钮1
        {2, ButtonFunctionType::SPEED_BOOST, "", true, 10, true},        // 按钮2
        {3, ButtonFunctionType::SPEED_SLOW, "", true, 10, true},        // 按钮3
    };
    
    return config;
}

// ============================================================================
// JoystickMappingProcessor 实现
// ============================================================================

JoystickMappingProcessor::JoystickMappingProcessor(std::shared_ptr<JoystickMappingConfig> mapping_config)
    : mapping_config_(mapping_config) {
    if (mapping_config_) {
        // 统一格式：12个轴（4个摇杆+8个旋钮），32个按钮/开关
        axis_values_.resize(MAX_JOYSTICK_AXES, 0.0);
        button_values_.resize(MAX_JOYSTICK_BUTTONS, 0);
    }
}

double JoystickMappingProcessor::processAxis(int physical_axis_index, double raw_value) {
    if (!mapping_config_ || physical_axis_index < 0 || physical_axis_index >= MAX_JOYSTICK_AXES) {
        return 0.0;
    }
    
    // 更新轴值缓存
    axis_values_[physical_axis_index] = raw_value;
    
    // 查找对应的轴映射配置
    for (const auto& axis_mapping : mapping_config_->axis_mappings) {
        if (axis_mapping.physical_axis_index == physical_axis_index && axis_mapping.enabled) {
            double processed_value = raw_value;
            
            // 应用死区
            if (std::abs(processed_value) < axis_mapping.deadzone) {
                processed_value = 0.0;
            } else {
                // 重新映射剩余范围
                double sign = (processed_value > 0) ? 1.0 : -1.0;
                double abs_value = std::abs(processed_value);
                double normalized_value = (abs_value - axis_mapping.deadzone) / (1.0 - axis_mapping.deadzone);
                processed_value = sign * normalized_value;
            }
            
            // 应用缩放因子
            processed_value *= axis_mapping.scale_factor;
            
            // 应用反转
            if (axis_mapping.invert) {
                processed_value = -processed_value;
            }
            
            // 限制范围
            processed_value = std::max(axis_mapping.min_value, std::min(axis_mapping.max_value, processed_value));
            
            // 更新映射后的轴值
            mapped_axis_values_[axis_mapping.mapping_type] = processed_value;
            
            return processed_value;
        }
    }
    
    return raw_value;
}

int JoystickMappingProcessor::processButton(int physical_button_index, int raw_value) {
    if (!mapping_config_ || physical_button_index < 0 || physical_button_index >= MAX_JOYSTICK_BUTTONS) {
        return 0;
    }
    
    // 更新按钮值缓存
    button_values_[physical_button_index] = raw_value;
    
    // 查找对应的按钮映射配置
    for (const auto& button_mapping : mapping_config_->button_mappings) {
        if (button_mapping.physical_button_index == physical_button_index && button_mapping.enabled) {
            int processed_value = raw_value;
            
            // 应用有效电平设置
            if (!button_mapping.active_high) {
                processed_value = (processed_value == 0) ? 1 : 0;
            }
            
            // 更新映射后的按钮状态
            mapped_button_states_[button_mapping.function_type] = (processed_value != 0);
            
            return processed_value;
        }
    }
    
    return raw_value;
}

double JoystickMappingProcessor::getMappedAxisValue(AxisMappingType mapping_type) {
    auto it = mapped_axis_values_.find(mapping_type);
    if (it != mapped_axis_values_.end()) {
        return it->second;
    }
    return 0.0;
}

bool JoystickMappingProcessor::getMappedButtonState(ButtonFunctionType function_type) {
    auto it = mapped_button_states_.find(function_type);
    if (it != mapped_button_states_.end()) {
        return it->second;
    }
    return false;
}

void JoystickMappingProcessor::updateMappingConfig(std::shared_ptr<JoystickMappingConfig> mapping_config) {
    mapping_config_ = mapping_config;
    if (mapping_config_) {
        // 统一格式：12个轴（4个摇杆+8个旋钮），32个按钮/开关
        axis_values_.resize(MAX_JOYSTICK_AXES, 0.0);
        button_values_.resize(MAX_JOYSTICK_BUTTONS, 0);
    }
    reset();
}

void JoystickMappingProcessor::reset() {
    std::fill(axis_values_.begin(), axis_values_.end(), 0.0);
    std::fill(button_values_.begin(), button_values_.end(), 0);
    mapped_axis_values_.clear();
    mapped_button_states_.clear();
}

} // namespace sealien_joystick
