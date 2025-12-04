# Sealien-CtrlSmt-Joystick 项目结构说明

## 概述

本项目已重新组织为模块化结构，按照功能职责进行清晰的文件夹划分，便于维护和扩展。

## 项目结构

```
Sealien-CtrlSmt-Joystick/
├── src/                          # 源代码目录
│   ├── core/                     # 核心应用控制模块
│   │   ├── main.cpp              # 程序入口点
│   │   └── application_controller.cpp  # 应用控制器
│   ├── config/                   # 配置管理模块
│   │   ├── config.cpp            # 基础配置加载器
│   │   ├── smart_config.cpp      # 智能配置管理器
│   │   └── usb_vendor_config.cpp # USB厂商配置加载器
│   ├── detection/                # 设备检测模块
│   │   ├── usb_serial_detector.cpp      # USB串口检测器
│   │   ├── smart_serial_detector.cpp     # 智能串口检测器
│   │   └── joystick_detector.cpp         # 手柄检测器
│   ├── communication/            # 通信模块
│   │   ├── joystick/                       # 手柄通信子模块
│   │   │   └── joystick_manager.cpp         # 手柄管理器
│   │   └── modbus/                          # Modbus通信子模块
│   │       ├── modbus_client.cpp            # Modbus客户端
│   │       └── modbus_data_processor.cpp    # Modbus数据处理器
│   ├── input/                    # 输入处理模块
│   │   ├── joystick_mapping.cpp  # 手柄映射处理
│   │   └── uinput_device.cpp      # 虚拟输入设备
│   └── utils/                    # 工具模块
│       ├── filters.cpp            # 数据过滤器
│       ├── mapping.cpp            # 映射工具
│       └── utils.cpp              # 通用工具函数
├── include/                      # 头文件目录
│   ├── core/                     # 核心模块头文件
│   ├── config/                   # 配置模块头文件
│   ├── detection/                # 检测模块头文件
│   ├── communication/            # 通信模块头文件
│   │   ├── joystick/                       # 手柄通信子模块头文件
│   │   │   └── joystick_manager.h           # 手柄管理器头文件
│   │   └── modbus/                          # Modbus子模块头文件
│   │       ├── modbus_client.h              # Modbus客户端头文件
│   │       └── modbus_data_processor.h     # Modbus数据处理器头文件
│   ├── input/                    # 输入模块头文件
│   └── utils/                    # 工具模块头文件
├── config/                       # 配置文件目录
│   ├── modbus/                   # Modbus配置文件
│   ├── joystick/                 # 手柄配置文件
│   ├── usb/                      # USB设备配置文件
│   │   └── usb_vendors.yaml     # USB厂商配置文件
├── docs/                         # 文档目录
├── build/                        # 构建目录
└── CMakeLists.txt               # CMake构建配置
```

## 模块说明

### 1. 核心应用控制模块 (core/)
- **main.cpp**: 程序入口点，负责初始化应用程序
- **application_controller.cpp**: 应用控制器，协调各个模块的工作

### 2. 配置管理模块 (config/)
- **config.cpp**: 基础配置加载器，支持YAML/JSON格式
- **smart_config.cpp**: 智能配置管理器，集成动态串口检测
- **usb_vendor_config.cpp**: USB厂商配置加载器，支持自定义厂商规则

### 3. 设备检测模块 (detection/)
- **usb_serial_detector.cpp**: USB串口设备检测器
- **smart_serial_detector.cpp**: 智能串口检测器，支持多种厂商芯片
- **joystick_detector.cpp**: 物理手柄设备检测器

### 4. 通信模块 (communication/)
- **joystick/joystick_manager.cpp**: 手柄管理器，处理手柄数据
- **modbus/modbus_client.cpp**: Modbus RTU客户端
- **modbus/modbus_data_processor.cpp**: Modbus数据处理器

### 5. 输入处理模块 (input/)
- **joystick_mapping.cpp**: 手柄映射处理，支持自定义映射
- **uinput_device.cpp**: Linux虚拟输入设备，创建虚拟手柄

### 6. 工具模块 (utils/)
- **filters.cpp**: 数据过滤器，提供各种滤波算法
- **mapping.cpp**: 映射工具，处理数据转换
- **utils.cpp**: 通用工具函数

## 关键特性

### 智能串口检测
- 支持多种USB转串口芯片：FTDI、CP210x、CH34x、CDC ACM、PL2303等
- 基于配置文件的厂商规则系统
- 自动设备优先级排序
- 热插拔支持

### 配置系统
- 支持YAML和JSON格式
- 动态配置更新
- USB厂商自定义规则
- 智能串口自动检测

### 通信协议
- Modbus RTU协议支持
- ROS2消息发布
- 自动重连机制
- 错误恢复处理

### 输入处理
- 物理手柄支持
- 虚拟手柄创建
- 自定义映射配置
- 多轴数据处理

## 编译说明

项目使用CMake构建系统，支持以下目标：

```bash
# 编译项目
make

# 清理构建文件
make clean

# 深度清理
make clean-all

# 重新配置
make reconfigure

# 创建部署包
make package

# 安装到系统
make install
```

## 配置文件

### USB厂商配置 (config/usb/usb_vendors.yaml)
定义各种USB转串口芯片的厂商ID、产品ID和优先级：

```yaml
vendors:
  ftdi:
    name: "FTDI"
    vendor_id: "0403"
    products:
      - product_id: "6001"
        name: "FT232R"
        priority: 100
        chip_type: "FTDI_FT232R"
```

### Modbus配置 (config/modbus/)
支持智能串口检测：

```yaml
serial_port: auto   # 智能检测模式
baud: 115200
slave_id: 1
```

## 扩展指南

### 添加新的USB厂商
1. 在 `config/usb/usb_vendors.yaml` 中添加厂商信息
2. 定义产品ID和优先级
3. 无需修改代码，重启程序即可生效

### 添加新的通信协议
1. 在 `src/communication/` 下创建新的子模块
2. 实现相应的客户端和处理器
3. 在 `application_controller.cpp` 中集成

### 添加新的输入设备
1. 在 `src/detection/` 下添加检测器
2. 在 `src/input/` 下添加处理逻辑
3. 更新配置文件支持

## 维护说明

- 各模块职责清晰，便于独立维护
- 配置文件与代码分离，支持动态配置
- 模块间通过接口通信，降低耦合度
- 统一的日志和错误处理机制
