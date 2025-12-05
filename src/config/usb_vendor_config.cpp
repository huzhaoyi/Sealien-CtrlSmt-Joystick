/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-10-23 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-10-23 11:29:37
 * @FilePath: src/core/usb_vendor_config.cpp
 * @Description: USB厂商配置加载器实现文件
 * 从配置文件加载USB厂商信息，支持动态配置
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "usb_vendor_config.h"
#include "utils.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <algorithm>
#include <regex>

USBVendorConfig::USBVendorConfig() {
    initializeDefaultConfig();
}

USBVendorConfig::~USBVendorConfig() {
}

bool USBVendorConfig::loadConfig(const std::string& config_path) {
    try {
        config_path_ = config_path;
        
        // 检查文件是否存在
        std::ifstream file(config_path);
        if (!file.good()) {
            DEBUG_CORE_WARNING("USB 厂商配置文件未找到: " << config_path << "，使用默认配置");
            return true; // 使用默认配置
        }
        file.close();

        // 加载YAML配置文件
        YAML::Node config = YAML::LoadFile(config_path);
        
        // 解析厂商配置
        if (config["vendors"]) {
            parseVendorsConfig(config["vendors"]);
        }
        
        // 解析匹配规则配置
        if (config["device_rules"]) {
            parseMatchRulesConfig(config["device_rules"]);
        }
        
        // 解析过滤规则配置
        if (config["device_filters"]) {
            parseFilterRulesConfig(config["device_filters"]);
        }
        
        DEBUG_CORE_LOG("USB 厂商配置已成功从以下路径加载: " << config_path);
        DEBUG_CORE_LOG("已加载 " << vendors_.size() << " 个厂商, " 
                      << match_rules_.size() << " 个匹配规则, " 
                      << filter_rules_.size() << " filter rules");
        
        return true;
        
    } catch (const std::exception& e) {
        DEBUG_CORE_ERROR("无法加载 USB 厂商配置: " << e.what());
        return false;
    }
}

const USBVendorInfo* USBVendorConfig::getVendorInfo(const std::string& vendor_id) const {
    auto it = vendors_.find(vendor_id);
    if (it != vendors_.end()) {
        return &it->second;
    }
    return nullptr;
}

const USBProductInfo* USBVendorConfig::getProductInfo(const std::string& vendor_id, const std::string& product_id) const {
    const USBVendorInfo* vendor = getVendorInfo(vendor_id);
    if (!vendor) {
        return nullptr;
    }
    
    auto it = vendor->products.find(product_id);
    if (it != vendor->products.end()) {
        return &it->second;
    }
    return nullptr;
}

std::string USBVendorConfig::identifyChipType(const std::string& vendor_id, 
                                            const std::string& product_id, 
                                            const std::string& driver_name) const {
    // 首先尝试匹配具体的产品
    const USBProductInfo* product = getProductInfo(vendor_id, product_id);
    if (product) {
        return product->chip_type;
    }
    
    // 然后尝试匹配厂商的默认芯片类型
    const USBVendorInfo* vendor = getVendorInfo(vendor_id);
    if (vendor && !vendor->chip_type.empty()) {
        return vendor->chip_type;
    }
    
    // 最后尝试匹配驱动名称模式
    for (const auto& [vendor_key, vendor_info] : vendors_) {
        if (!vendor_info.driver_pattern.empty() && 
            matchesPattern(driver_name, vendor_info.driver_pattern)) {
            return vendor_info.chip_type;
        }
    }
    
    // 检查通用规则
    for (const auto& rule : match_rules_) {
        if (rule.enabled && !rule.driver_pattern.empty() && 
            matchesPattern(driver_name, rule.driver_pattern)) {
            // 从规则名称推断芯片类型
            if (rule.name.find("CDC") != std::string::npos) {
                return "CDC_ACM";
            } else if (rule.name.find("Generic") != std::string::npos) {
                return "Generic_USB_Serial";
            }
        }
    }
    
    return "Unknown";
}

int USBVendorConfig::calculateDevicePriority(const std::string& vendor_id, 
                                           const std::string& product_id, 
                                           const std::string& driver_name) const {
    int priority = 0;
    
    // 首先尝试匹配具体的产品
    const USBProductInfo* product = getProductInfo(vendor_id, product_id);
    if (product) {
        priority = product->priority;
    } else {
        // 然后尝试匹配厂商的默认优先级
        const USBVendorInfo* vendor = getVendorInfo(vendor_id);
        if (vendor) {
            priority = vendor->priority;
        }
    }
    
    // 根据匹配规则计算额外优先级
    for (const auto& rule : match_rules_) {
        if (!rule.enabled) continue;
        
        bool matches = true;
        
        // 检查厂商ID
        if (!rule.vendor_id.empty() && vendor_id != rule.vendor_id) {
            matches = false;
        }
        
        // 检查产品ID
        if (!rule.product_id.empty() && product_id != rule.product_id) {
            matches = false;
        }
        
        // 检查驱动名称模式
        if (!rule.driver_pattern.empty() && !matchesPattern(driver_name, rule.driver_pattern)) {
            matches = false;
        }
        
        if (matches) {
            priority += rule.priority;
        }
    }
    
    return priority;
}

bool USBVendorConfig::shouldFilterDevice(const std::string& driver_name) const {
    for (const auto& filter : filter_rules_) {
        if (filter.enabled && matchesPattern(driver_name, filter.driver_pattern)) {
            return true;
        }
    }
    return false;
}

void USBVendorConfig::addVendorInfo(const USBVendorInfo& vendor_info) {
    vendors_[vendor_info.vendor_id] = vendor_info;
}

void USBVendorConfig::addMatchRule(const DeviceMatchRule& rule) {
    match_rules_.push_back(rule);
}

void USBVendorConfig::addFilterRule(const DeviceFilterRule& rule) {
    filter_rules_.push_back(rule);
}

void USBVendorConfig::resetToDefault() {
    vendors_.clear();
    match_rules_.clear();
    filter_rules_.clear();
    initializeDefaultConfig();
}

std::string USBVendorConfig::getConfigStatus() const {
    std::ostringstream oss;
    oss << "USB Vendor Config Status:\n";
    oss << "  Config file: " << (config_path_.empty() ? "default" : config_path_) << "\n";
    oss << "  Vendors: " << vendors_.size() << "\n";
    oss << "  Match rules: " << match_rules_.size() << "\n";
    oss << "  Filter rules: " << filter_rules_.size() << "\n";
    
    if (!vendors_.empty()) {
        oss << "  Available vendors:\n";
        for (const auto& [vendor_id, vendor_info] : vendors_) {
            oss << "    " << vendor_id << ": " << vendor_info.name 
                << " (" << vendor_info.products.size() << " products)\n";
        }
    }
    
    return oss.str();
}

void USBVendorConfig::initializeDefaultConfig() {
    // 初始化默认厂商配置
    USBVendorInfo ftdi;
    ftdi.name = "FTDI";
    ftdi.description = "FTDI USB转串口芯片";
    ftdi.vendor_id = "0403";
    ftdi.priority = 100;
    ftdi.chip_type = "FTDI";
    
    USBProductInfo ftdi_ft232r;
    ftdi_ft232r.product_id = "6001";
    ftdi_ft232r.name = "FT232R";
    ftdi_ft232r.description = "FTDI FT232R USB转串口芯片";
    ftdi_ft232r.priority = 100;
    ftdi_ft232r.chip_type = "FTDI_FT232R";
    ftdi.products["6001"] = ftdi_ft232r;
    
    vendors_["0403"] = ftdi;
    
    // 初始化默认匹配规则
    DeviceMatchRule ftdi_rule;
    ftdi_rule.name = "FTDI_Priority";
    ftdi_rule.description = "FTDI设备优先";
    ftdi_rule.vendor_id = "0403";
    ftdi_rule.priority = 20;
    ftdi_rule.enabled = true;
    match_rules_.push_back(ftdi_rule);
    
    DEBUG_CORE_LOG("默认 USB 厂商配置已初始化");
}

void USBVendorConfig::parseVendorsConfig(const YAML::Node& vendors_node) {
    for (const auto& vendor_pair : vendors_node) {
        std::string vendor_key = vendor_pair.first.as<std::string>();
        const YAML::Node& vendor_node = vendor_pair.second;
        
        USBVendorInfo vendor_info;
        vendor_info.name = vendor_node["name"].as<std::string>();
        vendor_info.description = vendor_node["description"].as<std::string>();
        vendor_info.vendor_id = vendor_node["vendor_id"].as<std::string>();
        
        if (vendor_node["driver_pattern"]) {
            vendor_info.driver_pattern = vendor_node["driver_pattern"].as<std::string>();
        }
        
        if (vendor_node["priority"]) {
            vendor_info.priority = vendor_node["priority"].as<int>();
        }
        
        if (vendor_node["chip_type"]) {
            vendor_info.chip_type = vendor_node["chip_type"].as<std::string>();
        }
        
        // 解析产品信息
        if (vendor_node["products"]) {
            for (const auto& product_node : vendor_node["products"]) {
                USBProductInfo product_info;
                product_info.product_id = product_node["product_id"].as<std::string>();
                product_info.name = product_node["name"].as<std::string>();
                product_info.description = product_node["description"].as<std::string>();
                product_info.priority = product_node["priority"].as<int>();
                product_info.chip_type = product_node["chip_type"].as<std::string>();
                
                vendor_info.products[product_info.product_id] = product_info;
            }
        }
        
        vendors_[vendor_info.vendor_id] = vendor_info;
    }
}

void USBVendorConfig::parseMatchRulesConfig(const YAML::Node& rules_node) {
    for (const auto& rule_node : rules_node) {
        DeviceMatchRule rule;
        rule.name = rule_node["name"].as<std::string>();
        rule.description = rule_node["description"].as<std::string>();
        rule.priority = rule_node["priority"].as<int>();
        rule.enabled = rule_node["enabled"].as<bool>();
        
        if (rule_node["vendor_id"]) {
            rule.vendor_id = rule_node["vendor_id"].as<std::string>();
        }
        
        if (rule_node["product_id"]) {
            rule.product_id = rule_node["product_id"].as<std::string>();
        }
        
        if (rule_node["driver_pattern"]) {
            rule.driver_pattern = rule_node["driver_pattern"].as<std::string>();
        }
        
        match_rules_.push_back(rule);
    }
}

void USBVendorConfig::parseFilterRulesConfig(const YAML::Node& filters_node) {
    for (const auto& filter_node : filters_node) {
        DeviceFilterRule filter;
        filter.name = filter_node["name"].as<std::string>();
        filter.description = filter_node["description"].as<std::string>();
        filter.driver_pattern = filter_node["driver_pattern"].as<std::string>();
        filter.enabled = filter_node["enabled"].as<bool>();
        
        filter_rules_.push_back(filter);
    }
}

bool USBVendorConfig::matchesPattern(const std::string& text, const std::string& pattern) const {
    try {
        std::regex pattern_regex(pattern);
        return std::regex_search(text, pattern_regex);
    } catch (const std::regex_error& e) {
        // 如果正则表达式无效，使用简单的字符串匹配
        return text.find(pattern) != std::string::npos;
    }
}
