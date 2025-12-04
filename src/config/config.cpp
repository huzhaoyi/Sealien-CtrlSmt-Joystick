/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:29:24
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:29:24
 * @FilePath: src/config.cpp
 * @Description: 配置文件加载和解析模块
 * 支持YAML和JSON格式的配置文件，解析Modbus摇杆的配置参数
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "config.h"
#include "mapping.h"
#include "utils.h"
#include "utils/debug_config.h"
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <map>

using json = nlohmann::json;

/**
 * @brief 检查字符串是否以指定后缀结尾（不区分大小写）
 * @param s 待检查的字符串
 * @param suffix 后缀字符串
 * @return 如果s以suffix结尾返回true，否则返回false
 */
static bool ends_with_ci(const std::string& s, const std::string& suffix) {
    if (s.size() < suffix.size()) return false;
    for (size_t i=0;i<suffix.size();++i){
        char a = static_cast<char>(std::tolower(static_cast<unsigned char>(s[s.size()-suffix.size()+i])));
        char b = static_cast<char>(std::tolower(static_cast<unsigned char>(suffix[i])));
        if (a!=b) return false;
    }
    return true;
}

/**
 * @brief 解析轴数据源类型字符串
 * @param t 类型字符串
 * @return 对应的AxisSourceKind枚举值
 * @throws std::runtime_error 当类型字符串无效时
 */
static AxisSourceKind parse_axis_src_kind(const std::string& t){
    if (t=="input_register") return AxisSourceKind::InputRegister;
    if (t=="holding_register") return AxisSourceKind::HoldingRegister;
    throw std::runtime_error("axis source.type must be input_register/holding_register");
}

/**
 * @brief 解析按钮数据源类型字符串
 * @param t 类型字符串
 * @return 对应的ButtonSourceKind枚举值
 * @throws std::runtime_error 当类型字符串无效时
 */
static ButtonSourceKind parse_btn_src_kind(const std::string& t){
    if (t=="coil") return ButtonSourceKind::Coil;
    if (t=="discrete_input") return ButtonSourceKind::DiscreteInput;
    if (t=="input_register_bit") return ButtonSourceKind::InputRegisterBit;
    if (t=="holding_register_bit") return ButtonSourceKind::HoldingRegisterBit;
    throw std::runtime_error("button/discrete source.type invalid");
}

/**
 * @brief 从JSON对象解析配置
 * @param j JSON对象
 * @return 解析后的Config对象
 */
static Config from_json(const json& j){
    Config c;
    
    // 辅助lambda函数：安全获取JSON值，提供默认值
    auto getS = [&](const char* k, const std::string& d){ return j.contains(k)? j[k].get<std::string>() : d; };
    auto getI = [&](const char* k, int d){ return j.contains(k)? j[k].get<int>() : d; };
    auto getD = [&](const char* k, double d){ return j.contains(k)? j[k].get<double>() : d; };

    // 解析串口通信参数
    c.serial_port = getS("serial_port", c.serial_port);
    
    // 处理智能串口检测配置
    if (c.serial_port == "auto" || c.serial_port == "smart") {
        c.serial_port = DEFAULT_SERIAL_PORT;  // 使用默认值，稍后由智能检测器更新
        DEBUG_CONFIG_LOG("Smart serial detection enabled (auto mode)");
    }
    
    c.baud        = getI("baud", c.baud);
    std::string p = getS("parity", std::string(1,c.parity));
    c.parity      = p.empty() ? 'N' : p[0];
    c.data_bits   = getI("data_bits", c.data_bits);
    c.stop_bits   = getI("stop_bits", c.stop_bits);
    
    // 解析从站ID：支持单个值或列表
    if (j.contains("slave_id")) {
        if (j["slave_id"].is_array()) {
            // 列表格式：slave_id: [1, 2]
            c.slave_ids.clear();
            for (const auto& id : j["slave_id"]) {
                int sid = id.get<int>();
                if (sid < 1 || sid > 247) {
                    throw std::runtime_error("slave_id invalid. expected 1-247");
                }
                c.slave_ids.push_back(sid);
            }
            if (c.slave_ids.empty()) {
                throw std::runtime_error("slave_id list cannot be empty");
            }
            c.slave_id = c.slave_ids[0];  // 保持向后兼容
        } else {
            // 单个值格式：slave_id: 1
            c.slave_id = j["slave_id"].get<int>();
            c.slave_ids = {c.slave_id};  // 转换为列表
        }
    } else {
        // 使用默认值
        c.slave_id = DEFAULT_SLAVE_ID;
        c.slave_ids = {DEFAULT_SLAVE_ID};
    }
    
    c.poll_hz     = getD("poll_hz", c.poll_hz);
    c.enable_modbus = j.value("enable_modbus", c.enable_modbus);
    
    // 解析阀控板配置
    c.enable_valve_control = j.value("enable_valve_control", c.enable_valve_control);
    if (j.contains("valve_control_config_file")) {
        c.valve_control_config_file = j["valve_control_config_file"].get<std::string>();
    }

    // 解析阀控板串口通信参数（独立串口）
    if (j.contains("valve_control_port")) {
        c.valve_control_port = j["valve_control_port"].get<std::string>();
    }
    if (j.contains("valve_control_baud")) {
        c.valve_control_baud = j["valve_control_baud"].get<int>();
    }
    if (j.contains("valve_control_parity")) {
        std::string parity_str = j["valve_control_parity"].get<std::string>();
        if (parity_str.length() > 0) {
            c.valve_control_parity = parity_str[0];
        }
    }
    if (j.contains("valve_control_data_bits")) {
        c.valve_control_data_bits = j["valve_control_data_bits"].get<int>();
    }
    if (j.contains("valve_control_stop_bits")) {
        c.valve_control_stop_bits = j["valve_control_stop_bits"].get<int>();
    }
    if (j.contains("valve_control_slave_id")) {
        c.valve_control_slave_id = j["valve_control_slave_id"].get<int>();
    }

    // 解析电压映射参数（必须通过配置文件设置）
    if (j.contains("voltage_mapping")) {
        auto& vm = j["voltage_mapping"];
        c.min_voltage = vm.value("min_voltage", 0.0);
        c.mid_voltage = vm.value("mid_voltage", 0.0);
        c.max_voltage = vm.value("max_voltage", 0.0);
        c.modbus_max_value = vm.value("modbus_max_value", 4095);  // 默认4095，对应标准12位ADC
        
        // 验证电压参数是否有效
        if (c.min_voltage <= 0.0 || c.mid_voltage <= 0.0 || c.max_voltage <= 0.0) {
            throw std::runtime_error("电压映射参数无效：min_voltage, mid_voltage, max_voltage 必须大于0");
        }
        if (c.min_voltage >= c.max_voltage) {
            throw std::runtime_error("电压映射参数无效：min_voltage 必须小于 max_voltage");
        }
        if (c.mid_voltage < c.min_voltage || c.mid_voltage > c.max_voltage) {
            throw std::runtime_error("电压映射参数无效：mid_voltage 必须在 min_voltage 和 max_voltage 之间");
        }
        // 验证Modbus最大值（uint16_t范围是0-65535，但0无效）
        if (c.modbus_max_value == 0) {
            throw std::runtime_error("电压映射参数无效：modbus_max_value 必须大于0");
        }
    } else {
        throw std::runtime_error("配置文件中缺少 voltage_mapping 参数，必须设置 min_voltage, mid_voltage, max_voltage");
    }


    // 解析连续轴配置
    if (j.contains("axes")){
        for (size_t i=0; i<j["axes"].size(); ++i){
            auto& a = j["axes"][i];
            AxisMap am;
            am.code = StrToAbsCode(a["code"].get<std::string>(), std::string("axes[")+std::to_string(i)+"].code");
            auto s = a["source"];
            am.src.kind = parse_axis_src_kind(s["type"].get<std::string>());
            am.src.addr = s["addr"].get<int>();
            am.raw_min  = a.value("raw_min",0);           // 原始值最小值，默认0
            am.raw_max  = a.value("raw_max",4095);        // 原始值最大值，默认4095
            am.deadzone = a.value("deadzone",0.0);        // 死区，默认0.0
            am.lpf_alpha= a.value("lpf_alpha",1.0);       // 低通滤波系数，默认1.0
            am.invert   = a.value("invert",false);        // 是否反转，默认false
            am.swap_endian = a.value("swap_endian",false); // 是否交换字节序，默认false
            am.ros2_joy_index = a.value("ros2_joy_index", static_cast<int>(i)); // ROS2 Joy轴索引，默认按顺序
            am.lpf.setAlpha(am.lpf_alpha);
            c.axes.push_back(am);
        }
    }

    // 解析按钮配置
    if (j.contains("buttons")){
        for (size_t i=0; i<j["buttons"].size(); ++i){
            auto& b = j["buttons"][i];
            ButtonMap bm;
            bm.code = StrToBtnCode(b["code"].get<std::string>(), std::string("buttons[")+std::to_string(i)+"].code");
            bm.active_high = b.value("active_high", true);  // 有效电平，默认高电平有效
            bm.debouncer.setDebounce(b.value("debounce_ms", 10));  // 去抖时间，默认10ms
            bm.ros2_joy_index = b.value("ros2_joy_index", static_cast<int>(i)); // ROS2 Joy按钮索引，默认按顺序
            auto s = b["source"];
            bm.src.kind = parse_btn_src_kind(s["type"].get<std::string>());
            bm.src.addr = s["addr"].get<int>();
            bm.src.bit  = s.value("bit", -1);  // 位索引，-1表示整个寄存器
            c.buttons.push_back(bm);
        }
    }

    // 解析离散轴配置
    if (j.contains("discrete_axes")){
        for (size_t i=0; i<j["discrete_axes"].size(); ++i){
            auto& d = j["discrete_axes"][i];
            DiscreteAxis da;
            da.code = StrToAbsCode(d["code"].get<std::string>(), std::string("discrete_axes[")+std::to_string(i)+"].code");
            da.lpf_alpha = d.value("lpf_alpha",1.0);  // 低通滤波系数，默认1.0
            da.lpf.setAlpha(da.lpf_alpha);

            // 解析上方向源
            auto up = d["up"];
            da.up.kind = parse_btn_src_kind(up["type"].get<std::string>());
            da.up.addr = up["addr"].get<int>();
            da.up.bit  = up.value("bit",-1);

            // 解析下方向源
            auto dn = d["down"];
            da.down.kind = parse_btn_src_kind(dn["type"].get<std::string>());
            da.down.addr = dn["addr"].get<int>();
            da.down.bit  = dn.value("bit",-1);

            c.discrete_axes.push_back(da);
        }
    }

    // 调试配置现在从单独的配置文件加载，不再从设备配置文件中读取
    // 保持debug_config字段的默认值即可
    c.debug_config.setDefaults();
    // 参数合法性校验
    auto validate = [](const Config& cfg){
        // 串口基础参数
        auto is_std_baud = [](int b){
            switch (b) {
                case 1200: case 2400: case 4800: case 9600: case 19200: case 38400: case 57600: case 115200:
                    return true;
                default: return false;
            }
        };
        if (cfg.serial_port.empty()) throw std::runtime_error("serial_port is empty");
        if (!is_std_baud(cfg.baud)) throw std::runtime_error("baud invalid. expected one of 1200/2400/4800/9600/19200/38400/57600/115200");
        if (!(cfg.parity=='N' || cfg.parity=='E' || cfg.parity=='O')) throw std::runtime_error("parity invalid. expected N/E/O");
        if (cfg.data_bits < MIN_DATA_BITS || cfg.data_bits > MAX_DATA_BITS) throw std::runtime_error("data_bits invalid. expected " + std::to_string(MIN_DATA_BITS) + "-" + std::to_string(MAX_DATA_BITS));
        if (!(cfg.stop_bits==MIN_STOP_BITS || cfg.stop_bits==MAX_STOP_BITS)) throw std::runtime_error("stop_bits invalid. expected " + std::to_string(MIN_STOP_BITS) + " or " + std::to_string(MAX_STOP_BITS));
        // 验证从站ID（单个值和列表）
        if (cfg.slave_id < 1 || cfg.slave_id > 247) throw std::runtime_error("slave_id invalid. expected 1-247");
        for (int sid : cfg.slave_ids) {
            if (sid < 1 || sid > 247) throw std::runtime_error("slave_id in list invalid. expected 1-247");
        }
        if (!(cfg.poll_hz > MIN_POLL_HZ && cfg.poll_hz <= MAX_POLL_HZ)) throw std::runtime_error("poll_hz invalid. expected (" + std::to_string(MIN_POLL_HZ) + ", " + std::to_string(MAX_POLL_HZ) + "]");

        // 连续轴
        for (const auto& a : cfg.axes){
            if (!(a.raw_min < a.raw_max)) throw std::runtime_error("axis raw_min < raw_max required");
            if (!(a.deadzone >= 0.0 && a.deadzone <= 0.49)) throw std::runtime_error("axis deadzone invalid. expected [0.0, 0.49]");
            if (!(a.lpf_alpha >= 0.0 && a.lpf_alpha <= 1.0)) throw std::runtime_error("axis lpf_alpha invalid. expected [0.0, 1.0]");
            if (a.src.addr < 0) throw std::runtime_error("axis source.addr invalid. expected >= 0");
        }

        auto validate_bit_source = [](const RegBitSource& s){
            if (s.addr < 0) throw std::runtime_error("source.addr invalid. expected >= 0");
            if (s.kind==ButtonSourceKind::Coil || s.kind==ButtonSourceKind::DiscreteInput){
                // coil 和 discrete_input 不需要 bit，应为-1或未设置
                if (s.bit != -1) throw std::runtime_error("coil/discrete_input source should not specify bit (use -1)");
            } else {
                if (s.bit < 0 || s.bit > 15) throw std::runtime_error("register bit index invalid. expected 0-15");
            }
        };

        // 按钮
        for (const auto& b : cfg.buttons){
            validate_bit_source(b.src);
        }

        // 离散轴
        for (const auto& d : cfg.discrete_axes){
            if (!(d.lpf_alpha >= 0.0 && d.lpf_alpha <= 1.0)) throw std::runtime_error("discrete lpf_alpha invalid. expected [0.0, 1.0]");
            validate_bit_source(d.up);
            validate_bit_source(d.down);
        }
    };

    validate(c);
    return c;
}

/**
 * @brief 从文件加载配置
 * @param path 配置文件路径
 * @return 解析后的Config对象
 * @throws std::runtime_error 当文件无法打开或解析失败时
 */
Config ConfigLoader::Load(const std::string& path){
    // 根据文件扩展名选择解析方式
    if (ends_with_ci(path, ".json")){
        // JSON格式配置文件
        std::ifstream ifs(path);
        if (!ifs) throw std::runtime_error("Cannot open JSON config: " + path);
        json j; ifs >> j;
        return from_json(j);
    } else {
        // YAML格式配置文件（转换为JSON后解析）
        YAML::Node y = YAML::LoadFile(path);
        json j = json::object();

        // 解析标量参数
        if (y["serial_port"]) {
            std::string serial_port = y["serial_port"].as<std::string>();
            // 处理智能串口检测配置
            if (serial_port == "auto" || serial_port == "smart") {
                j["serial_port"] = DEFAULT_SERIAL_PORT;  // 使用默认值，稍后由智能检测器更新
                DEBUG_CONFIG_LOG("Smart serial detection enabled (auto mode)");
            } else {
                j["serial_port"] = serial_port;
            }
        }
        if (y["baud"]) j["baud"] = y["baud"].as<int>();
        if (y["parity"]) j["parity"] = y["parity"].as<std::string>();
        if (y["data_bits"]) j["data_bits"] = y["data_bits"].as<int>();
        if (y["stop_bits"]) j["stop_bits"] = y["stop_bits"].as<int>();
        
        // 解析从站ID：支持单个值或列表
        if (y["slave_id"]) {
            if (y["slave_id"].IsSequence()) {
                // 列表格式：slave_id: [1, 2]
                json slave_id_array = json::array();
                for (const auto& id : y["slave_id"]) {
                    slave_id_array.push_back(id.as<int>());
                }
                j["slave_id"] = slave_id_array;
            } else {
                // 单个值格式：slave_id: 1
                j["slave_id"] = y["slave_id"].as<int>();
            }
        }
        
        if (y["poll_hz"]) j["poll_hz"] = y["poll_hz"].as<double>();
        if (y["enable_modbus"]) j["enable_modbus"] = y["enable_modbus"].as<bool>();

        // 解析阀控板配置
        if (y["enable_valve_control"]) {
            j["enable_valve_control"] = y["enable_valve_control"].as<bool>();
        }
        if (y["valve_control_config_file"]) {
            j["valve_control_config_file"] = y["valve_control_config_file"].as<std::string>();
        }
        
        // 解析阀控板串口通信参数（独立串口）
        if (y["valve_control"]) {
            auto vc = y["valve_control"];
            if (vc["serial_port"]) {
                std::string serial_port = vc["serial_port"].as<std::string>();
                // 处理智能串口检测配置
                if (serial_port == "auto" || serial_port == "smart") {
                    j["valve_control_port"] = DEFAULT_SERIAL_PORT;  // 使用默认值，稍后由智能检测器更新
                    DEBUG_CONFIG_LOG("Valve control: Smart serial detection enabled (auto mode)");
                } else {
                    j["valve_control_port"] = serial_port;
                }
            }
            if (vc["baud"]) j["valve_control_baud"] = vc["baud"].as<int>();
            if (vc["parity"]) j["valve_control_parity"] = vc["parity"].as<std::string>();
            if (vc["data_bits"]) j["valve_control_data_bits"] = vc["data_bits"].as<int>();
            if (vc["stop_bits"]) j["valve_control_stop_bits"] = vc["stop_bits"].as<int>();
            if (vc["slave_id"]) j["valve_control_slave_id"] = vc["slave_id"].as<int>();
        }

        // 解析电压映射参数（必须通过配置文件设置）
        if (y["voltage_mapping"]) {
            auto vm = y["voltage_mapping"];
            json voltage_mapping;
            
            // 检查必需的电压参数
            if (!vm["min_voltage"] || !vm["mid_voltage"] || !vm["max_voltage"]) {
                throw std::runtime_error("配置文件中 voltage_mapping 缺少必需的参数：min_voltage, mid_voltage, max_voltage");
            }
            
            voltage_mapping["min_voltage"] = vm["min_voltage"].as<double>();
            voltage_mapping["mid_voltage"] = vm["mid_voltage"].as<double>();
            voltage_mapping["max_voltage"] = vm["max_voltage"].as<double>();
            // Modbus寄存器最大值（可选，默认4095）
            if (vm["modbus_max_value"]) {
                voltage_mapping["modbus_max_value"] = vm["modbus_max_value"].as<int>();
            } else {
                voltage_mapping["modbus_max_value"] = 4095;  // 默认值
            }
            j["voltage_mapping"] = voltage_mapping;
        } else {
            throw std::runtime_error("配置文件中缺少 voltage_mapping 参数，必须设置 min_voltage, mid_voltage, max_voltage");
        }

        // 解析连续轴配置
        if (y["axes"]){
            j["axes"] = json::array();
            for (auto n : y["axes"]){
                json a;
                a["code"] = n["code"].as<std::string>();
                auto s = n["source"];
                a["source"] = {{"type", s["type"].as<std::string>()}, {"addr", s["addr"].as<int>()}};
                if (n["raw_min"]) a["raw_min"] = n["raw_min"].as<int>();
                if (n["raw_max"]) a["raw_max"] = n["raw_max"].as<int>();
                if (n["deadzone"]) a["deadzone"] = n["deadzone"].as<double>();
                if (n["lpf_alpha"]) a["lpf_alpha"] = n["lpf_alpha"].as<double>();
                if (n["invert"]) a["invert"] = n["invert"].as<bool>();
                if (n["swap_endian"]) a["swap_endian"] = n["swap_endian"].as<bool>();
                if (n["ros2_joy_index"]) a["ros2_joy_index"] = n["ros2_joy_index"].as<int>();
                j["axes"].push_back(a);
            }
        }

        // 调试配置现在从单独的配置文件加载，不再从设备配置文件中读取

        // 解析按钮配置
        if (y["buttons"]){
            j["buttons"] = json::array();
            for (auto n : y["buttons"]){
                json b;
                b["code"] = n["code"].as<std::string>();
                b["active_high"] = n["active_high"] ? n["active_high"].as<bool>() : true;
                if (n["debounce_ms"]) b["debounce_ms"] = n["debounce_ms"].as<int>();
                if (n["ros2_joy_index"]) b["ros2_joy_index"] = n["ros2_joy_index"].as<int>();
                auto s = n["source"];
                json src;
                src["type"] = s["type"].as<std::string>();
                src["addr"] = s["addr"].as<int>();
                if (s["bit"]) src["bit"] = s["bit"].as<int>();
                b["source"] = src;
                j["buttons"].push_back(b);
            }
        }

        // 解析离散轴配置
        if (y["discrete_axes"]){
            j["discrete_axes"] = json::array();
            for (auto n : y["discrete_axes"]){
                json d;
                d["code"] = n["code"].as<std::string>();
                if (n["lpf_alpha"]) d["lpf_alpha"] = n["lpf_alpha"].as<double>();
                auto up = n["up"];
                auto dn = n["down"];
                d["up"] = {{"type", up["type"].as<std::string>()}, {"addr", up["addr"].as<int>()}};
                if (up["bit"]) d["up"]["bit"] = up["bit"].as<int>();
                d["down"] = {{"type", dn["type"].as<std::string>()}, {"addr", dn["addr"].as<int>()}};
                if (dn["bit"]) d["down"]["bit"] = dn["bit"].as<int>();
                j["discrete_axes"].push_back(d);
            }
        }

        return from_json(j);
    }
}
