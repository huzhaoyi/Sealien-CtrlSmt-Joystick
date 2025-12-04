/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-09-02 15:30:00
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-09-02 15:30:00
 * @FilePath: include/joystick/joystick_mapping.h
 * @Description: 手柄映射功能头文件
 * 解决不同USB手柄（如XBOX、PS、罗技等）的轴/按钮索引差异问题
 * 实现用户自定义输入（物理手柄的轴/按钮）与输出（ROS2话题/Twist控制）的映射关系
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <linux/input.h>
#include "../core/constants.h"

namespace sealien_joystick {

/**
 * @brief 手柄厂商类型枚举
 */
enum class JoystickVendor {
    UNKNOWN = 0,
    XBOX,           ///< Xbox手柄
    PLAYSTATION,    ///< PlayStation手柄
    LOGITECH,       ///< 罗技手柄
    THRUSTMASTER,   ///< Thrustmaster手柄
    SAITEK,         ///< Saitek手柄
    GENERIC         ///< 通用手柄
};

/**
 * @brief 手柄产品类型枚举
 */
enum class JoystickProduct {
    UNKNOWN = 0,
    XBOX_360_CONTROLLER,
    XBOX_ONE_CONTROLLER,
    XBOX_SERIES_CONTROLLER,
    PS3_CONTROLLER,
    PS4_CONTROLLER,
    PS5_CONTROLLER,
    LOGITECH_F310,
    LOGITECH_F710,
    LOGITECH_GAMEPAD_F310,
    THRUSTMASTER_T16000M,
    GENERIC_GAMEPAD
};

/**
 * @brief 轴映射类型枚举
 */
enum class AxisMappingType {
    LINEAR_X,       ///< 线速度X轴
    LINEAR_Y,       ///< 线速度Y轴
    LINEAR_Z,       ///< 线速度Z轴
    ANGULAR_X,      ///< 角速度X轴
    ANGULAR_Y,      ///< 角速度Y轴
    ANGULAR_Z,      ///< 角速度Z轴
    CUSTOM          ///< 自定义轴
};

/**
 * @brief 按钮功能类型枚举
 */
enum class ButtonFunctionType {
    EMERGENCY_STOP, ///< 紧急停止
    MODE_SWITCH,    ///< 模式切换
    SPEED_BOOST,    ///< 速度提升
    SPEED_SLOW,     ///< 速度降低
    CUSTOM          ///< 自定义功能
};

/**
 * @brief 手柄设备信息结构
 */
struct JoystickDeviceInfo {
    std::string device_path;      ///< 设备路径
    std::string device_name;      ///< 设备名称
    std::string vendor_name;      ///< 厂商名称
    std::string product_name;    ///< 产品名称
    uint16_t vendor_id;           ///< 厂商ID
    uint16_t product_id;         ///< 产品ID
    uint16_t version;            ///< 版本号
    int num_axes;               ///< 轴数量
    int num_buttons;            ///< 按钮数量
    JoystickVendor vendor;      ///< 手柄厂商类型
    JoystickProduct product;    ///< 手柄产品类型
    bool is_connected;          ///< 是否已连接
};

/**
 * @brief 轴映射配置结构
 */
struct AxisMappingConfig {
    int physical_axis_index;     ///< 物理轴索引
    AxisMappingType mapping_type; ///< 映射类型
    std::string custom_name;     ///< 自定义轴名称
    bool invert;                 ///< 是否反转轴值
    double deadzone;             ///< 死区大小
    double scale_factor;         ///< 缩放因子
    double min_value;            ///< 最小值
    double max_value;            ///< 最大值
    bool enabled;                ///< 是否启用
};

/**
 * @brief 按钮映射配置结构
 */
struct ButtonMappingConfig {
    int physical_button_index;   ///< 物理按钮索引
    ButtonFunctionType function_type; ///< 功能类型
    std::string custom_name;    ///< 自定义按钮名称
    bool active_high;            ///< 高电平有效
    int debounce_ms;            ///< 去抖时间（毫秒）
    bool enabled;               ///< 是否启用
};

/**
 * @brief 手柄映射配置结构
 */
struct JoystickMappingConfig {
    // 设备识别信息
    uint16_t vendor_id;         ///< 厂商ID
    uint16_t product_id;        ///< 产品ID
    std::string device_name;    ///< 设备名称
    std::string config_name;    ///< 配置名称
    
    // 轴映射配置
    std::vector<AxisMappingConfig> axis_mappings;
    
    // 按钮映射配置
    std::vector<ButtonMappingConfig> button_mappings;
    
    // 全局配置
    double default_deadzone;     ///< 默认死区
    double default_scale_factor; ///< 默认缩放因子
    bool enable_auto_detection;  ///< 是否启用自动检测
};

/**
 * @brief 手柄映射管理器类
 * 
 * 负责管理不同手柄的映射配置，提供自动检测和手动配置功能
 */
class JoystickMappingManager {
public:
    /**
     * @brief 构造函数
     */
    JoystickMappingManager();
    
    /**
     * @brief 析构函数
     */
    ~JoystickMappingManager();
    
    /**
     * @brief 从文件加载映射配置
     * @param config_file 配置文件路径
     * @return 加载成功返回true
     */
    bool loadMappingConfig(const std::string& config_file);
    
    /**
     * @brief 保存映射配置到文件
     * @param config_file 配置文件路径
     * @return 保存成功返回true
     */
    bool saveMappingConfig(const std::string& config_file);
    
    /**
     * @brief 自动检测手柄类型并应用默认映射
     * @param device_info 设备信息
     * @return 检测成功返回true
     */
    bool autoDetectAndMap(const JoystickDeviceInfo& device_info);
    
    /**
     * @brief 获取手柄的映射配置
     * @param vendor_id 厂商ID
     * @param product_id 产品ID
     * @return 映射配置指针，未找到返回nullptr
     */
    std::shared_ptr<JoystickMappingConfig> getMappingConfig(uint16_t vendor_id, uint16_t product_id);
    
    /**
     * @brief 获取手柄的映射配置（通过设备名称）
     * @param device_name 设备名称
     * @return 映射配置指针，未找到返回nullptr
     */
    std::shared_ptr<JoystickMappingConfig> getMappingConfig(const std::string& device_name);
    
    /**
     * @brief 添加或更新映射配置
     * @param config 映射配置
     * @return 操作成功返回true
     */
    bool addOrUpdateMappingConfig(const JoystickMappingConfig& config);
    
    /**
     * @brief 删除映射配置
     * @param vendor_id 厂商ID
     * @param product_id 产品ID
     * @return 删除成功返回true
     */
    bool removeMappingConfig(uint16_t vendor_id, uint16_t product_id);
    
    /**
     * @brief 获取所有映射配置
     * @return 映射配置列表
     */
    std::vector<std::shared_ptr<JoystickMappingConfig>> getAllMappingConfigs() const;
    
    /**
     * @brief 创建默认映射配置
     * @param device_info 设备信息
     * @return 默认映射配置
     */
    JoystickMappingConfig createDefaultMapping(const JoystickDeviceInfo& device_info);
    
    /**
     * @brief 验证映射配置的有效性
     * @param config 映射配置
     * @return 配置有效返回true
     */
    bool validateMappingConfig(const JoystickMappingConfig& config);
    
    /**
     * @brief 获取手柄厂商类型
     * @param vendor_id 厂商ID
     * @return 厂商类型
     */
    static JoystickVendor getVendorType(uint16_t vendor_id);
    
    /**
     * @brief 获取手柄产品类型
     * @param vendor_id 厂商ID
     * @param product_id 产品ID
     * @return 产品类型
     */
    static JoystickProduct getProductType(uint16_t vendor_id, uint16_t product_id);
    
    /**
     * @brief 获取厂商名称
     * @param vendor 厂商类型
     * @return 厂商名称
     */
    static std::string getVendorName(JoystickVendor vendor);
    
    /**
     * @brief 获取产品名称
     * @param product 产品类型
     * @return 产品名称
     */
    static std::string getProductName(JoystickProduct product);

private:
    /**
     * @brief 初始化默认映射配置
     */
    void initializeDefaultMappings();
    
    /**
     * @brief 创建Xbox手柄默认映射
     * @return Xbox手柄默认映射配置
     */
    JoystickMappingConfig createXboxDefaultMapping();
    
    /**
     * @brief 创建PlayStation手柄默认映射
     * @return PlayStation手柄默认映射配置
     */
    JoystickMappingConfig createPlayStationDefaultMapping();
    
    /**
     * @brief 创建罗技手柄默认映射
     * @return 罗技手柄默认映射配置
     */
    JoystickMappingConfig createLogitechDefaultMapping();
    
    /**
     * @brief 创建通用手柄默认映射
     * @return 通用手柄默认映射配置
     */
    JoystickMappingConfig createGenericDefaultMapping();
    
    std::map<std::string, std::shared_ptr<JoystickMappingConfig>> device_mappings_; ///< 设备名称到映射配置的映射
    std::map<std::pair<uint16_t, uint16_t>, std::shared_ptr<JoystickMappingConfig>> id_mappings_; ///< 厂商ID+产品ID到映射配置的映射
    std::string config_file_path_; ///< 配置文件路径
};

/**
 * @brief 手柄映射处理器类
 * 
 * 负责处理手柄输入数据的映射转换
 */
class JoystickMappingProcessor {
public:
    /**
     * @brief 构造函数
     * @param mapping_config 映射配置
     */
    JoystickMappingProcessor(std::shared_ptr<JoystickMappingConfig> mapping_config);
    
    /**
     * @brief 处理轴数据
     * @param physical_axis_index 物理轴索引
     * @param raw_value 原始值
     * @return 处理后的值
     */
    double processAxis(int physical_axis_index, double raw_value);
    
    /**
     * @brief 处理按钮数据
     * @param physical_button_index 物理按钮索引
     * @param raw_value 原始值
     * @return 处理后的值
     */
    int processButton(int physical_button_index, int raw_value);
    
    /**
     * @brief 获取映射后的轴值
     * @param mapping_type 映射类型
     * @return 映射后的轴值
     */
    double getMappedAxisValue(AxisMappingType mapping_type);
    
    /**
     * @brief 获取映射后的按钮状态
     * @param function_type 功能类型
     * @return 按钮状态
     */
    bool getMappedButtonState(ButtonFunctionType function_type);
    
    /**
     * @brief 更新映射配置
     * @param mapping_config 新的映射配置
     */
    void updateMappingConfig(std::shared_ptr<JoystickMappingConfig> mapping_config);
    
    /**
     * @brief 重置所有数据
     */
    void reset();

private:
    std::shared_ptr<JoystickMappingConfig> mapping_config_; ///< 映射配置
    std::vector<double> axis_values_; ///< 轴值缓存
    std::vector<int> button_values_; ///< 按钮值缓存
    std::map<AxisMappingType, double> mapped_axis_values_; ///< 映射后的轴值
    std::map<ButtonFunctionType, bool> mapped_button_states_; ///< 映射后的按钮状态
};

} // namespace sealien_joystick
