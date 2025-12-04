/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-10-23 11:29:37
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-10-23 11:29:37
 * @FilePath: include/core/usb_vendor_config.h
 * @Description: USB厂商配置加载器头文件
 * 从配置文件加载USB厂商信息，支持动态配置
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#ifndef USB_VENDOR_CONFIG_H
#define USB_VENDOR_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <yaml-cpp/yaml.h>

/**
 * @brief USB产品信息结构
 */
struct USBProductInfo {
    std::string product_id;      // 产品ID
    std::string name;            // 产品名称
    std::string description;     // 产品描述
    int priority;               // 优先级
    std::string chip_type;      // 芯片类型
};

/**
 * @brief USB厂商信息结构
 */
struct USBVendorInfo {
    std::string name;            // 厂商名称
    std::string description;     // 厂商描述
    std::string vendor_id;       // 厂商ID
    std::string driver_pattern; // 驱动名称模式（可选）
    int priority;               // 默认优先级
    std::string chip_type;      // 默认芯片类型
    std::map<std::string, USBProductInfo> products; // 产品列表
};

/**
 * @brief 设备匹配规则结构
 */
struct DeviceMatchRule {
    std::string name;            // 规则名称
    std::string description;     // 规则描述
    std::string vendor_id;       // 厂商ID（可选）
    std::string product_id;      // 产品ID（可选）
    std::string driver_pattern; // 驱动名称模式（可选）
    int priority;               // 优先级
    bool enabled;               // 是否启用
};

/**
 * @brief 设备过滤规则结构
 */
struct DeviceFilterRule {
    std::string name;            // 规则名称
    std::string description;     // 规则描述
    std::string driver_pattern; // 驱动名称模式
    bool enabled;               // 是否启用
};

/**
 * @brief USB厂商配置类
 * 
 * 该类负责从配置文件加载USB厂商信息，支持：
 * - 动态加载厂商配置
 * - 设备匹配和过滤
 * - 优先级计算
 * - 芯片类型识别
 */
class USBVendorConfig {
public:
    using VendorMap = std::map<std::string, USBVendorInfo>;
    using RuleList = std::vector<DeviceMatchRule>;
    using FilterList = std::vector<DeviceFilterRule>;

    /**
     * @brief 构造函数
     */
    USBVendorConfig();

    /**
     * @brief 析构函数
     */
    ~USBVendorConfig();

    /**
     * @brief 从配置文件加载厂商信息
     * @param config_path 配置文件路径
     * @return 加载成功返回true，失败返回false
     */
    bool loadConfig(const std::string& config_path);

    /**
     * @brief 获取厂商信息
     * @param vendor_id 厂商ID
     * @return 厂商信息指针，未找到返回nullptr
     */
    const USBVendorInfo* getVendorInfo(const std::string& vendor_id) const;

    /**
     * @brief 获取产品信息
     * @param vendor_id 厂商ID
     * @param product_id 产品ID
     * @return 产品信息指针，未找到返回nullptr
     */
    const USBProductInfo* getProductInfo(const std::string& vendor_id, const std::string& product_id) const;

    /**
     * @brief 识别设备芯片类型
     * @param vendor_id 厂商ID
     * @param product_id 产品ID
     * @param driver_name 驱动名称
     * @return 芯片类型字符串
     */
    std::string identifyChipType(const std::string& vendor_id, 
                                const std::string& product_id, 
                                const std::string& driver_name) const;

    /**
     * @brief 计算设备优先级
     * @param vendor_id 厂商ID
     * @param product_id 产品ID
     * @param driver_name 驱动名称
     * @return 优先级分数
     */
    int calculateDevicePriority(const std::string& vendor_id, 
                               const std::string& product_id, 
                               const std::string& driver_name) const;

    /**
     * @brief 检查设备是否应该被过滤
     * @param driver_name 驱动名称
     * @return 应该过滤返回true，否则返回false
     */
    bool shouldFilterDevice(const std::string& driver_name) const;

    /**
     * @brief 获取所有厂商信息
     * @return 厂商信息映射
     */
    const VendorMap& getAllVendors() const { return vendors_; }

    /**
     * @brief 获取所有匹配规则
     * @return 匹配规则列表
     */
    const RuleList& getMatchRules() const { return match_rules_; }

    /**
     * @brief 获取所有过滤规则
     * @return 过滤规则列表
     */
    const FilterList& getFilterRules() const { return filter_rules_; }

    /**
     * @brief 添加自定义厂商信息
     * @param vendor_info 厂商信息
     */
    void addVendorInfo(const USBVendorInfo& vendor_info);

    /**
     * @brief 添加自定义匹配规则
     * @param rule 匹配规则
     */
    void addMatchRule(const DeviceMatchRule& rule);

    /**
     * @brief 添加自定义过滤规则
     * @param rule 过滤规则
     */
    void addFilterRule(const DeviceFilterRule& rule);

    /**
     * @brief 重置为默认配置
     */
    void resetToDefault();

    /**
     * @brief 获取配置状态信息
     * @return 状态信息字符串
     */
    std::string getConfigStatus() const;

private:
    VendorMap vendors_;          // 厂商信息映射
    RuleList match_rules_;       // 匹配规则列表
    FilterList filter_rules_;    // 过滤规则列表
    std::string config_path_;    // 配置文件路径

    /**
     * @brief 初始化默认配置
     */
    void initializeDefaultConfig();

    /**
     * @brief 解析厂商配置
     * @param vendors_node YAML节点
     */
    void parseVendorsConfig(const YAML::Node& vendors_node);

    /**
     * @brief 解析匹配规则配置
     * @param rules_node YAML节点
     */
    void parseMatchRulesConfig(const YAML::Node& rules_node);

    /**
     * @brief 解析过滤规则配置
     * @param filters_node YAML节点
     */
    void parseFilterRulesConfig(const YAML::Node& filters_node);

    /**
     * @brief 检查字符串是否匹配模式
     * @param text 待检查的字符串
     * @param pattern 模式字符串
     * @return 匹配返回true，否则返回false
     */
    bool matchesPattern(const std::string& text, const std::string& pattern) const;
};

#endif // USB_VENDOR_CONFIG_H
