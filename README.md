# 🎮 Sealien-CtrlSmt-Joystick

## 📋 项目简介

智能工业手柄转换器，支持USB物理手柄和Modbus工业设备的自动检测与数据转换。系统能够实时检测设备连接状态，动态切换运行模式，并将数据发布到ROS2话题或创建Linux虚拟输入设备。

## ✨ 核心功能

### 🔌 智能设备检测
- **USB转串口自动检测**: 支持FTDI、CP210x、CH34x、CDC ACM、PL2303等多种厂商芯片
- **即插即用**: 无需修改配置，自动识别和适配不同厂商的USB转串口设备
- **热插拔支持**: 设备插拔时自动切换，无需重启程序
- **智能优先级**: 根据芯片类型和规则自动选择最佳设备

### 🎮 手柄数据处理
- **物理手柄检测**: 自动识别和管理USB手柄设备
- **智能映射**: 处理不同手柄的轴/按钮索引差异
- **虚拟设备**: 创建Linux uinput虚拟输入设备
- **实时处理**: 支持高频率数据更新和实时响应

### 📡 Modbus通信
- **Modbus RTU协议**: 完整的Modbus RTU客户端实现
- **自动重连**: 断线后自动重连，支持错误恢复
- **数据转换**: 将Modbus数据转换为标准手柄格式
- **多设备支持**: 支持多种Modbus设备配置

### 🤖 ROS2集成
- **话题发布**: 自动发布手柄数据到ROS2话题
- **消息类型**: 支持Joy、Twist、String等标准消息类型
- **域管理**: 智能ROS2域ID检测和配置
- **实时监控**: 提供ROS2话题监听和调试工具

### ⚙️ 配置系统
- **YAML/JSON配置**: 灵活的配置文件支持
- **智能串口检测**: 支持"auto"模式自动选择串口
- **USB厂商配置**: 基于配置文件的厂商规则系统
- **动态配置**: 支持运行时配置更新

## 🚀 快速开始

### 🎯 快速启动

#### **统一启动脚本 (推荐)**
```bash
# 启动程序 (自动选择配置文件 + 自动检测ROS2域ID)
./run.sh

# 指定配置文件
./run.sh config/modbus/AMSAMOTION_IO8R.yaml

# 指定ROS2域ID
./run.sh config/modbus/AMSAMOTION_IO8R.yaml 1
```

#### **直接运行**
```bash
# USB模式 (无需配置文件)
./build/sealien-ctrlsmt-joystick

# Modbus模式
./build/sealien-ctrlsmt-joystick config/modbus/AMSAMOTION_IO8R.yaml
```

### 📋 系统要求

- **操作系统**: Linux (Ubuntu 18.04+)
- **依赖库**: 
  - libmodbus (Modbus通信)
  - yaml-cpp (配置文件解析)
  - ROS2 Humble (可选，ROS2功能)
- **权限**: 需要uinput设备访问权限

### 🔧 安装依赖

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install libmodbus-dev libyaml-cpp-dev

# ROS2 (可选)
sudo apt install ros-humble-desktop
```

## 📖 详细文档

- [项目结构说明](docs/PROJECT_STRUCTURE.md) - 详细的模块化架构说明
- [配置指南](docs/CONFIGURATION_GUIDE.md) - 配置文件使用指南
- [设备支持](docs/DEVICE_SUPPORT.md) - 支持的设备和芯片列表
- [API参考](docs/API_REFERENCE.md) - 开发接口参考
- [故障排除](docs/TROUBLESHOOTING.md) - 常见问题解决方案

## 🏗️ 项目架构

```
src/
├── core/                    # 核心应用控制
├── config/                  # 配置管理模块
├── detection/               # 设备检测模块
├── communication/          # 通信模块
├── input/                   # 输入处理模块
└── utils/                   # 工具模块
```

## 🔧 编译说明

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

## 📝 配置示例

### USB转串口智能检测
```yaml
# Modbus配置
serial_port: auto   # 智能检测模式，自动选择最佳串口设备
baud: 115200
slave_id: 1
enable_modbus: true
```

### USB厂商自定义规则
```yaml
# config/usb/usb_vendors.yaml
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

## 🤝 贡献指南

1. Fork 项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开 Pull Request

## 📄 许可证

本项目采用专有许可证。未经授权，禁止复制、分发或修改。

## 📞 支持

如有问题或建议，请联系：
- 邮箱: hu418@163.com
- 项目地址: [Sealien-CtrlSmt-Joystick](https://github.com/your-repo/sealien-ctrlsmt-joystick)

---

**Code By SRS-HUZY Compile**  
**Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.**
