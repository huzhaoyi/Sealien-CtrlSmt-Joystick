# 🎮 Sealien-CtrlSmt-Joystick

## 📋 项目简介

智能工业手柄转换器，支持USB物理手柄和Modbus工业设备的自动检测与数据转换。系统能够实时检测设备连接状态，动态切换运行模式，并将数据发布到ROS2话题或创建Linux虚拟输入设备。

## ✨ 核心功能

### 🔌 设备支持
- **USB手柄检测**: 自动识别和管理物理USB手柄设备
- **Modbus设备**: 支持Modbus RTU协议读取工业设备数据
- **动态切换**: 实时检测设备连接状态，自动在USB和Modbus模式间切换

### 🎮 数据处理
- **手柄映射**: 智能映射不同USB手柄的轴/按钮索引差异
- **数据转换**: 将Modbus数据转换为标准手柄格式
- **虚拟设备**: 创建Linux uinput虚拟输入设备
- **实时处理**: 支持高频率数据更新和实时响应

### 🤖 ROS2集成
- **话题发布**: 自动发布手柄数据到ROS2话题
- **消息类型**: 支持Joy、Twist、String等标准消息类型
- **域管理**: 智能ROS2域ID检测和配置
- **实时监控**: 提供ROS2话题监听和调试工具

### 🌐 Web监控
- **实时仪表板**: 现代化Web界面监控系统状态
- **数据可视化**: 手柄轴/按钮、Modbus数据、ROS2话题的实时显示
- **系统日志**: 实时日志监控和错误诊断
- **配置管理**: 可视化配置参数显示

### ⚙️ 配置管理
- **YAML配置**: 灵活的YAML配置文件支持
- **自动选择**: 智能配置文件自动选择机制
- **参数调优**: 死区、滤波、映射等参数可配置
- **多设备支持**: 支持多种手柄和Modbus设备配置

## 🚀 快速开始

### 🎯 快速启动

#### **统一启动脚本 (推荐)**
```bash
# 启动程序 (自动选择配置文件 + 自动检测ROS2域ID)
./run.sh

# 指定配置文件
./run.sh config/custom.yaml

# 指定ROS2域ID
./run.sh -d 1

# 自动检测可用域ID
./run.sh -a

# 禁用ROS2功能
./run.sh --no-ros2

# 查看帮助
./run.sh -h
```

**自动配置文件选择**：
如果不指定配置文件，脚本会自动搜索`config/`目录中的所有`.yaml`文件，并按文件名排序选择第一个文件作为默认配置。这样您可以在`config/`目录中添加任意名称的YAML配置文件，脚本都能自动识别和使用。

#### **ROS2话题监听**
```bash
# 监听手柄数据话题
./run.sh -j

# 监听速度命令话题
./run.sh -t

# 监听设备状态话题
./run.sh -s

# 列出所有话题
./run.sh -l

# 显示话题信息
./run.sh -i /sealien_joystick

# 查看话题发布频率
./run.sh -f /sealien_joystick

# 只接收一次消息
./run.sh --once -j
```

#### **环境检查**
```bash
# 检查ROS2环境
./run.sh -c
```

### 🌐 Web监控仪表板

#### **启动Web仪表板**
```bash
# 进入web_control_panel目录
cd web_control_panel

# 启动Web控制面板
./scripts/start.sh
```

#### **访问Web界面**
启动后打开浏览器访问：
```
http://localhost:8080/frontend/web_dashboard.html
```

#### **Web仪表板功能**
- 🎮 **实时手柄数据**: 6个轴和16个按钮的实时状态显示
- 📡 **Modbus数据监控**: 8个模拟量输入通道电压监控
- 🤖 **ROS2话题监控**: 实时显示ROS2话题数据
- 📊 **系统状态**: USB手柄、Modbus设备、ROS2节点状态
- 📋 **实时日志**: 系统运行日志实时显示
- ⚙️ **配置信息**: 串口、电压映射、轴配置等参数显示

## 📋 系统要求

### **操作系统**
- **Linux**: Ubuntu 18.04+ / Debian 10+ / CentOS 7+
- **内核**: Linux 3.10+ (支持uinput和input子系统)
- **架构**: x86_64 / ARM64

### **依赖库**
- **CMake**: 3.16+
- **C++编译器**: GCC 7.0+ / Clang 5.0+
- **ROS2**: Humble (可选，用于ROS2功能)
- **Python3**: 3.6+ (用于Web仪表板)
- **系统库**: libmodbus, yaml-cpp, Linux input子系统

### **硬件要求**
- **USB端口**: 支持USB手柄连接
- **串口**: 支持Modbus RTU设备连接 (可选)
- **内存**: 最少512MB RAM
- **存储**: 最少100MB可用空间

### **权限要求**
- **input组**: 用户需要加入input组以访问手柄设备
- **串口权限**: 访问Modbus设备需要串口权限
- **sudo权限**: 创建虚拟设备需要sudo权限

### 🛠️ 构建项目

#### **推荐方式：使用Makefile**
```
# 查看所有可用目标
make help

# 标准构建
make build

# 调试模式构建
make debug

# 发布模式构建
make release

# 安静构建 (最小化警告)
make quiet

# 构建并运行
make run

# 清理构建
make clean          # 清理构建文件
make clean-all      # 深度清理构建目录

# 其他有用目标
make status         # 检查构建状态
make deps           # 检查系统依赖
make package        # 创建部署包
make install        # 安装到系统
```

#### **传统方式：CMake**
```
mkdir build && cd build
cmake .. && make
```

### 🚀 运行程序
```
# 推荐方式：构建并运行
make run

# 直接运行（自动检测模式）
./build/sealien-ctrlsmt-joystick [config_file]

# 使用默认配置文件（自动检测USB+Modbus+ROS2）
./build/sealien-ctrlsmt-joystick

# 指定配置文件（自动检测USB+Modbus+ROS2）
./build/sealien-ctrlsmt-joystick config/AMSAMOTION_IO8R.yaml

# 查看帮助信息
./build/sealien-ctrlsmt-joystick --help
```

## 🏗️ 项目结构

```
Sealien-CtrlSmt-Joystick/
├── 📁 include/                    # 头文件目录
│   ├── 📁 core/                   # 核心功能头文件
│   │   ├── constants.h            # 宏定义常量
│   │   ├── config.h               # 配置加载器
│   │   ├── application_controller.h # 应用程序主控制器
│   │   ├── modbus_data_processor.h  # Modbus数据处理器
│   │   └── joystick_manager.h     # USB手柄管理器
│   ├── 📁 modbus/                 # Modbus相关头文件
│   │   └── modbus_client.h        # Modbus客户端
│   ├── 📁 joystick/               # 手柄相关头文件
│   │   ├── joystick_detector.h    # 手柄检测器
│   │   ├── uinput_device.h        # 虚拟输入设备
│   │   ├── joystick_mapping.h     # 手柄映射功能
│   │   └── keycodes.h             # 输入事件代码
│   └── 📁 utils/                  # 工具类头文件
│       ├── utils.h                 # 工具函数
│       ├── filters.h               # 信号滤波器
│       ├── mapping.h               # 数据映射
│       ├── byte_order.h            # 字节序转换
│       └── voltage_converter.h     # 电压转换
├── 📁 src/                        # 源代码目录
│   ├── 📁 core/                   # 核心功能源文件
│   │   ├── main.cpp               # 主程序入口（简化后）
│   │   ├── config.cpp             # 配置加载器实现
│   │   ├── application_controller.cpp # 应用程序主控制器实现
│   │   ├── modbus_data_processor.cpp # Modbus数据处理器实现
│   │   └── joystick_manager.cpp   # USB手柄管理器实现
│   ├── 📁 modbus/                 # Modbus相关源文件
│   │   └── modbus_client.cpp      # Modbus客户端实现
│   ├── 📁 joystick/               # 手柄相关源文件
│   │   ├── joystick_detector.cpp  # 手柄检测器实现
│   │   ├── uinput_device.cpp      # 虚拟输入设备实现
│   │   └── joystick_mapping.cpp   # 手柄映射功能实现
│   └── 📁 utils/                  # 工具类源文件
│       ├── utils.cpp               # 工具函数实现
│       ├── filters.cpp             # 信号滤波器实现
│       └── mapping.cpp             # 数据映射实现
├── 📁 config/                     # 配置文件目录
│   ├── AMSAMOTION_IO8R.yaml      # Modbus设备配置文件
│   ├── joystick_detector.yaml    # 手柄检测器配置
│   ├── joystick_mappings.yaml    # 手柄映射配置文件
│   └── joystick_ros2_params.yaml # ROS2参数配置
├── 📁 web_control_panel/          # Web控制面板
│   ├── 📁 frontend/               # 前端文件
│   │   ├── web_dashboard.html     # Web界面主文件
│   │   └── favicon.svg            # 网站图标
│   ├── 📁 backend/                # 后端服务
│   │   └── ros2_websocket_server.py # ROS2 WebSocket服务器
│   ├── 📁 assets/                 # 静态资源
│   │   └── 📁 images/             # 图片资源
│   │       └── SEALIEN-LOGO.png   # 公司Logo
│   ├── 📁 config/                 # 配置文件
│   │   └── config.yaml.example    # 配置文件示例
│   ├── 📁 scripts/                # 脚本文件
│   │   └── start.sh               # 启动脚本
│   ├── 📁 docs/                   # 文档
│   │   ├── README.md              # Web控制面板说明文档
│   │   ├── INSTALL.md             # 安装指南
│   │   └── API.md                 # API文档
│   ├── 📁 tests/                  # 测试文件
│   ├── 📁 logs/                   # 日志文件目录
│   ├── requirements.txt            # Python依赖包列表
│   ├── Dockerfile                  # Docker容器配置
│   ├── docker-compose.yml          # Docker编排配置
│   ├── .gitignore                  # Git忽略文件
│   ├── LICENSE                     # 开源许可证
│   └── CHANGELOG.md                # 版本更新日志
├── 📁 build/                      # 构建输出目录
│   └── sealien-ctrlsmt-joystick-1.0.0 # 编译后的可执行文件
├── 📄 run.sh                      # 统一启动脚本（推荐使用）
├── 📄 Makefile                    # 构建管理脚本
├── 📄 CMakeLists.txt              # CMake构建配置
├── 📄 README.md                   # 项目说明文档
├── 📄 JOYSTICK_MAPPING_GUIDE.md   # 手柄映射使用指南
├── 📄 MODBUS_PROTOCOL_SPECIFICATION.md # Modbus协议规范
├── 📄 WEB_DASHBOARD_ORGANIZATION.md # Web仪表板组织说明
└── 📄 sealien-ctrlsmt-joystick.service # 系统服务文件
```

## 🎯 模块说明

### 🔧 **Core模块** (`include/core/`, `src/core/`)
- **功能**: 程序核心功能和架构控制
- **文件**: 
  - `constants.h` - 宏定义常量
  - `config.h/cpp` - 配置加载器
  - `main.cpp` - 主程序入口（简化后，只负责初始化和启动）
  - `application_controller.h/cpp` - 应用程序主控制器，协调各个模块
  - `modbus_data_processor.h/cpp` - Modbus数据处理器，处理Modbus数据读取和转换
  - `joystick_manager.h/cpp` - USB手柄管理器，管理物理手柄检测和事件处理

### 📡 **Modbus模块** (`include/modbus/`, `src/modbus/`)
- **功能**: Modbus通信功能
- **文件**: Modbus RTU客户端

### 🎮 **Joystick模块** (`include/joystick/`, `src/joystick/`)
- **功能**: 手柄检测、管理和映射
- **文件**: USB手柄检测器、虚拟输入设备、手柄映射功能、输入事件代码

### 🤖 **ROS2功能** (已整合到Core模块)
- **功能**: ROS2集成功能已整合到JoystickManager中
- **说明**: 不再需要独立的ROS2模块，功能已统一

### 🛠️ **Utils模块** (`include/utils/`, `src/utils/`)
- **功能**: 工具类和辅助功能
- **文件**: 通用工具函数、信号滤波、数据映射、字节序转换、电压转换

### 🌐 **Web控制面板模块** (`web_control_panel/`)
- **功能**: 现代化Web界面监控系统状态和数据
- **技术**: HTML5 + CSS3 + JavaScript + WebSocket + Python3
- **结构**: 
  - `frontend/`: 前端HTML/CSS/JS文件
  - `backend/`: Python WebSocket服务器
  - `assets/`: 静态资源文件
  - `scripts/`: 启动和管理脚本
  - `docs/`: 文档说明
- **特性**: 
  - 实时手柄数据可视化（6轴16按钮）
  - Modbus设备数据监控（8通道电压）
  - ROS2话题实时监控
  - 系统状态和日志显示
  - 配置参数可视化
- **启动方式**: 
  - 统一启动：`./start.sh`

## 🔧 配置说明

### **宏定义常量** (`include/core/constants.h`)

所有可配置参数都在宏定义中，包括：

#### **手柄和ROS2相关常量**
- `MAX_JOYSTICK_AXES` - 最大手柄轴数量 (8)
- `MAX_JOYSTICK_BUTTONS` - 最大手柄按钮数量 (16)
- `ROS2_NODE_NAME` - ROS2节点名称
- `ROS2_TOPIC_JOY` - 手柄数据话题 (`/sealien_joystick`)
- `ROS2_TOPIC_TWIST` - 速度控制话题 (`/sealien_joystick/cmd_vel`)
- `ROS2_TOPIC_STATUS` - 状态信息话题 (`/sealien_joystick/device_status`)

#### **时间间隔和频率常量**
- `JOYSTICK_SCAN_INTERVAL_MS` - 手柄扫描间隔 (100ms)
- `STATUS_DISPLAY_INTERVAL_COUNT` - 状态显示间隔 (50次)

#### **数据范围和精度常量**
- `AXIS_NORMALIZATION_FACTOR` - 轴数据归一化因子 (32767.0f)
- `MIN_POLL_HZ` - 最小轮询频率 (0.0)
- `MAX_POLL_HZ` - 最大轮询频率 (1000.0)

#### **摇杆死区处理常量**
- `USB_JOYSTICK_DEFAULT_DEADZONE` - USB摇杆默认死区阈值 (0.12)
- `USB_JOYSTICK_MIN_DEADZONE` - USB摇杆最小死区阈值 (0.05)
- `USB_JOYSTICK_MAX_DEADZONE` - USB摇杆最大死区阈值 (0.30)
- `USB_JOYSTICK_DEFAULT_SMOOTHING_FACTOR` - USB摇杆默认平滑系数 (0.3)
- `DEFAULT_DEADZONE` - Modbus轴默认死区阈值 (0.02)
- `MAX_DEADZONE` - 最大死区阈值 (0.49)
- `MIN_DEADZONE` - 最小死区阈值 (0.0)

#### **串口和通信常量**
- `MIN_DATA_BITS` - 最小数据位 (5)
- `MAX_DATA_BITS` - 最大数据位 (8)
- `MIN_STOP_BITS` - 最小停止位 (1)
- `MAX_STOP_BITS` - 最大停止位 (2)

#### **虚拟设备配置常量**
- `VIRTUAL_DEVICE_NAME` - 虚拟设备名称
- `DEFAULT_DEBOUNCE_MS` - 默认去抖时间 (50ms)

#### **Modbus通信相关常量**
- `MAX_BATCH_READ_COUNT` - 最大批量读取数量 (125)

### **配置文件** (`config/AMSAMOTION_IO8R.yaml`)

支持YAML格式的配置文件，配置Modbus通信参数和手柄映射。

### **手柄映射配置** (`config/joystick_mappings.yaml`)

解决不同USB手柄（如XBOX、PS、罗技等）的轴/按钮索引差异问题，实现用户自定义输入与输出的映射关系：

#### **支持的映射类型**
- **轴映射类型**: `LINEAR_X`, `LINEAR_Y`, `LINEAR_Z`, `ANGULAR_X`, `ANGULAR_Y`, `ANGULAR_Z`, `CUSTOM`
- **按钮功能类型**: `EMERGENCY_STOP`, `MODE_SWITCH`, `SPEED_BOOST`, `SPEED_SLOW`, `CUSTOM`

#### **配置结构**
```yaml
joystick_mappings:
  - vendor_id: 0x045E          # 厂商ID
    product_id: 0x02D1         # 产品ID
    device_name: "Xbox One Controller"
    config_name: "Xbox_One_Default"
    default_deadzone: 0.12    # 默认死区
    default_scale_factor: 1.0 # 默认缩放因子
    enable_auto_detection: true
    
    axis_mappings:
      - physical_axis_index: 0    # 物理轴索引
        mapping_type: 0           # 映射类型（LINEAR_X）
        custom_name: "left_stick_x"
        invert: false             # 是否反转
        deadzone: 0.12           # 死区大小
        scale_factor: 1.0         # 缩放因子
        min_value: -1.0          # 最小值
        max_value: 1.0           # 最大值
        enabled: true            # 是否启用
        
    button_mappings:
      - physical_button_index: 0  # 物理按钮索引
        function_type: 0          # 功能类型（EMERGENCY_STOP）
        custom_name: "button_a"
        active_high: true         # 高电平有效
        debounce_ms: 10          # 去抖时间
        enabled: true            # 是否启用
```

#### **自动检测功能**
- **厂商识别**: 自动识别Xbox、PlayStation、Logitech等主流手柄厂商
- **产品识别**: 自动识别具体的手柄型号（Xbox One、PS4、F310等）
- **默认映射**: 为每种手柄类型提供优化的默认映射配置
- **通用回退**: 对未知手柄使用通用映射配置

#### **支持的设备**
- **Xbox系列**: Xbox 360、Xbox One、Xbox Series
- **PlayStation系列**: PS3、PS4、PS5
- **Logitech系列**: F310、F710、Gamepad F310
- **其他**: Thrustmaster T16000M等
- **通用**: 自动回退到通用配置

### **ROS2节点参数** (`config/joystick_ros2_params.yaml`)

支持多手柄轴/按钮差异的参数化映射：

- **话题与功能开关**: `joystick_topic`, `twist_topic`, `device_status_topic`; `publishing.enable_joy|twist|status`
- **手柄映射功能**: `joystick_mapping.enabled`, `joystick_mapping.auto_detect`, `joystick_mapping.fallback_to_generic`
- **轴映射**: `axis_mapping.linear_x|linear_y|angular_z`（兼容旧配置）
- **按钮映射**: `button_mapping.emergency_stop|mode_switch|speed_boost`（兼容旧配置）
- **速度缩放**: `speed_scaling.slow_factor`, `speed_scaling.boost_factor`

示例：
```yaml
joystick_node:
  ros__parameters:
    # 手柄映射功能
    joystick_mapping:
      enabled: true
      auto_detect: true
      fallback_to_generic: true
    
    # 兼容旧配置的轴映射
    axis_mapping:
      linear_x: 1
      linear_y: 0
      angular_z: 3
    
    # 兼容旧配置的按钮映射
    button_mapping:
      emergency_stop: 0
      mode_switch: 4
      speed_boost: 5
    
    speed_scaling:
      slow_factor: 0.5
      boost_factor: 1.5
```

注意：手柄映射功能启用时，会优先使用映射配置文件中的设置，忽略兼容性配置。

### **摇杆死区处理**

项目实现了智能的摇杆死区处理机制，有效避免轻微触碰产生数据：

#### **死区处理原理**
- **死区内**: 轴值绝对值小于阈值时输出0
- **死区外**: 重新映射剩余范围到[-1, 1]，保持连续性
- **阈值限制**: 死区阈值限制在合理范围内，避免完全阻塞信号

#### **配置方式**
**宏定义配置** (`include/core/constants.h`)
```cpp
#define USB_JOYSTICK_DEFAULT_DEADZONE 0.12  // 12%默认死区（通用推荐）
#define USB_JOYSTICK_MIN_DEADZONE 0.05     // 5%最小死区
#define USB_JOYSTICK_MAX_DEADZONE 0.30     // 30%最大死区
```

> **注意**: 项目采用宏定义统一管理配置，ROS2节点直接使用宏定义值，无需额外的参数配置。

#### **建议设置**
- **精密控制**: 5-8% (0.05-0.08)
- **一般使用**: 8-12% (0.08-0.12)  
- **粗放控制**: 12-20% (0.12-0.20)

## 🔍 查找文件指南

### **查找特定功能**
- **应用程序控制**: `include/core/application_controller.*` + `src/core/application_controller.*`
- **Modbus数据处理**: `include/core/modbus_data_processor.*` + `src/core/modbus_data_processor.*`
- **USB手柄管理**: `include/core/joystick_manager.*` + `src/core/joystick_manager.*`
- **配置相关**: `include/core/config.*` + `src/core/config.*`
- **Modbus通信**: `include/modbus/` + `src/modbus/`
- **手柄功能**: `include/joystick/` + `src/joystick/`
- **ROS2集成**: `include/ros2/` + `src/ros2/`
- **工具函数**: `include/utils/` + `src/utils/`

### **调试特定模块**
- **应用程序启动问题**: 查看 `core/application_controller.*`
- **Modbus数据处理问题**: 查看 `core/modbus_data_processor.*`
- **USB手柄管理问题**: 查看 `core/joystick_manager.*`
- **手柄检测问题**: 查看 `joystick/` 目录
- **Modbus连接问题**: 查看 `modbus/` 目录
- **ROS2发布问题**: 查看 `ros2/` 目录
- **配置加载问题**: 查看 `core/config.*`

## 🎨 彩色日志系统

### **颜色规范**
系统使用ANSI颜色代码为不同模块和日志级别提供视觉区分：

#### **日志级别颜色**
- 🔴 **ERROR**: 红色粗体 - 错误和异常情况
- 🟡 **WARN**: 黄色粗体 - 警告和注意事项
- 🔵 **INFO**: 青色 - 一般信息和状态
- 🟣 **DEBUG**: 紫色 - 调试信息

#### **模块专用颜色**
- 🔵 **CORE**: 青色 - 核心模块日志
- 🔵 **MODBUS**: 蓝色 - 所有Modbus相关日志
- 🟢 **JOYSTICK**: 绿色 - 手柄检测和管理
- 🟣 **ROS2**: 紫色 - ROS2消息发布
- 🔵 **UINPUT**: 青色 - 虚拟输入设备
- 🟡 **CONFIG**: 黄色 - 配置加载和管理
- 🟡 **MAPPING**: 黄色 - 手柄映射功能
- 🟣 **UTILS**: 紫色 - 工具函数

#### **Modbus子模块**
- 🔵 **MODBUS-COMM**: 通信连接和错误
- 🔵 **MODBUS-INPUT**: 输入寄存器读取
- 🔵 **MODBUS-HOLDING**: 保持寄存器读取
- 🔵 **MODBUS-COIL**: 线圈状态读取
- 🔵 **MODBUS-DISCRETE**: 离散输入读取
- 🔵 **MODBUS-VOLTAGE**: 电压转换处理
- 🔵 **MODBUS-WRITE**: 写入操作

### **颜色效果**
- 错误信息以红色粗体显示，便于快速识别问题
- 警告信息以黄色粗体显示，提醒注意但不影响运行
- 不同模块使用不同颜色，便于区分日志来源
- 所有颜色在终端中清晰可见，支持大多数现代终端

### **统一日志系统**
- **统一使用DEBUG宏**: 所有模块都使用DEBUG_*_LOG宏，确保颜色支持
- **完整错误覆盖**: 所有警告和报错都有相应的日志输出
- **模块化日志**: 每个模块使用专用的DEBUG宏，便于调试和监控
- **流式输出支持**: 支持复杂的日志消息组合，如变量和字符串拼接
- **分级调试控制**: 支持精细的调试开关控制，可独立开启/关闭特定功能的数据打印

### **ROS2数据打印控制**
- **分级调试宏**: 像Modbus一样，ROS2数据打印也支持分级控制
- **独立开关**: 可分别控制轴数据、按钮数据、Twist消息等的打印
- **性能优化**: 关闭不需要的调试输出可提高程序性能
- **调试友好**: 便于开发时精确控制需要查看的数据类型

#### **可用的ROS2调试宏**
```cpp
#define DEBUG_ROS2_COMM 1      // 基础通信调试（连接、错误等）
#define DEBUG_ROS2_AXES 0      // 轴数据打印
#define DEBUG_ROS2_BUTTONS 0   // 按钮数据打印
#define DEBUG_ROS2_TWIST 0     // Twist消息打印
#define DEBUG_ROS2_STATUS 0    // 状态消息打印
#define DEBUG_ROS2_PUBLISH 0   // 发布操作日志
#define DEBUG_ROS2_SUBSCRIBE 0 // 订阅操作日志
```

## 🚀 核心特性

### **智能设备管理**
- ✅ **自动检测**: 自动检测USB手柄和Modbus设备，无需手动配置
- ✅ **动态切换**: 实时检测设备连接状态，自动在USB和Modbus模式间切换
- ✅ **多设备支持**: 同时支持USB手柄和Modbus设备的并行运行
- ✅ **热插拔支持**: 支持设备的热插拔检测和自动重连

### **数据处理与转换**
- ✅ **实时处理**: 高频率数据更新和实时响应
- ✅ **智能映射**: 解决不同USB手柄的轴/按钮索引差异问题
- ✅ **死区处理**: 智能死区过滤，避免轻微触碰产生数据
- ✅ **信号滤波**: 内置低通滤波器，平滑数据输出
- ✅ **数据转换**: 将Modbus数据转换为标准手柄格式

### **ROS2集成**
- ✅ **话题发布**: 自动发布手柄数据到ROS2话题
- ✅ **多消息类型**: 支持Joy、Twist、String等标准消息类型
- ✅ **域管理**: 智能ROS2域ID检测和配置
- ✅ **实时监控**: 提供ROS2话题监听和调试工具

### **Web监控系统**
- ✅ **实时仪表板**: 现代化Web界面监控系统状态
- ✅ **数据可视化**: 手柄轴/按钮、Modbus数据、ROS2话题的实时显示
- ✅ **系统日志**: 实时日志监控和错误诊断
- ✅ **配置管理**: 可视化配置参数显示

### **系统架构**
- ✅ **模块化设计**: 清晰的模块划分，便于维护和扩展
- ✅ **统一脚本**: 集成所有功能的统一启动脚本
- ✅ **配置管理**: 灵活的YAML配置文件支持
- ✅ **彩色日志**: 不同模块使用不同颜色，便于调试和监控
- ✅ **错误处理**: 完善的错误处理和日志系统

## 📚 使用示例

### **ROS2话题监听**

#### **USB手柄数据**
```
# 监听USB手柄数据
ros2 topic echo /joystick

# 监听速度控制命令
ros2 topic echo /cmd_vel

# 监听设备状态信息
ros2 topic echo /device_status
```

#### **Modbus设备数据**
系统现在同时支持USB手柄和Modbus设备的ROS2消息发布：

- **USB手柄**: 当检测到物理USB手柄时，会发布到 `/sealien_joystick` 话题
- **Modbus设备**: 当Modbus连接成功时，会以 `modbus_joystick` 为设备ID发布到 `/sealien_joystick` 话题
- **混合模式**: 可以同时连接USB手柄和Modbus设备，两者都会发布ROS2消息

#### **验证Modbus ROS2功能**
```bash
# 启动程序
./build/sealien-ctrlsmt-joystick config/AMSAMOTION_IO8R.yaml --enable-ros2

# 监听所有设备的手柄数据
ros2 topic echo /sealien_joystick

# 监听速度控制命令
ros2 topic echo /cmd_vel

# 监听设备状态
ros2 topic echo /device_status
```

#### **自动检测功能**
🎯 **智能检测**：
- **USB手柄**: 自动检测物理USB手柄连接/断开
- **Modbus设备**: 自动检测Modbus设备连接状态
- **动态切换**: 实时监控设备状态，自动切换运行模式
- **ROS2集成**: 自动启用ROS2功能，无需手动配置

### **Web仪表板监控**

#### **启动Web仪表板**
```bash
# 进入web_control_panel目录
cd web_control_panel

# 启动Web控制面板
./scripts/start.sh
```

#### **访问Web界面**
启动服务器后，打开浏览器访问：
```
http://localhost:8080/frontend/web_dashboard.html
```

#### **Web仪表板功能**
- **实时数据监控**: 手柄轴值、按钮状态、Modbus电压、ROS2话题
- **系统状态监控**: USB手柄、Modbus设备、ROS2节点连接状态
- **可视化界面**: 条形图、状态指示灯、实时日志
- **响应式设计**: 支持桌面和移动设备访问

#### **Web仪表板特性**
- **实时更新**: 数据每100ms更新一次
- **WebSocket通信**: 低延迟实时数据推送
- **多设备支持**: 同时监控USB手柄和Modbus设备
- **日志监控**: 实时显示系统运行日志
- **配置显示**: 显示系统配置参数

#### **运行模式**
- **USB模式**: 仅检测到USB手柄时运行
- **Modbus模式**: 仅检测到Modbus设备时运行  
- **混合模式**: 同时检测到USB手柄和Modbus设备时运行
- **动态模式**: 实时检测设备状态，自动切换模式

#### **测试结果**
✅ **功能验证完成**：
- Modbus设备成功发布ROS2消息，设备ID为 `modbus_joystick`
- 同时支持USB手柄和Modbus设备的ROS2消息发布
- Twist消息正常发布（测试中收到79个消息）
- Joy消息正常发布，包含正确的frame_id和数据结构
- 系统在动态检测模式下稳定运行

### **摇杆死区配置**
```
# 修改宏定义常量（需要重新编译）
# 在 include/core/constants.h 中修改：
# USB_JOYSTICK_DEFAULT_DEADZONE 0.12  # 12%默认死区
# USB_JOYSTICK_MIN_DEADZONE 0.05     # 5%最小死区
# USB_JOYSTICK_MAX_DEADZONE 0.30     # 30%最大死区
```

### **手柄映射功能使用**

#### **自动检测和映射**
```bash
# 启动程序，自动检测手柄类型并应用映射
./build/sealien-ctrlsmt-joystick config/AMSAMOTION_IO8R.yaml

# 查看检测到的手柄信息
ros2 topic echo /device_status
```

#### **自定义映射配置**
```yaml
# 在 config/joystick_mappings.yaml 中添加自定义映射
joystick_mappings:
  - vendor_id: 0x1234          # 您的设备厂商ID
    product_id: 0x5678         # 您的设备产品ID
    device_name: "My Custom Controller"
    config_name: "Custom_Mapping"
    
    axis_mappings:
      - physical_axis_index: 0
        mapping_type: 0        # LINEAR_X
        custom_name: "custom_x"
        invert: false
        deadzone: 0.10
        scale_factor: 1.2
        enabled: true
```

#### **ROS2参数配置**
```bash
# 启用手柄映射功能
ros2 run sealien_joystick sealien-ctrlsmt-joystick --ros-args \
  -p joystick_mapping.enabled:=true \
  -p joystick_mapping.auto_detect:=true \
  -p joystick_mapping.fallback_to_generic:=true
```

#### **快速开始**
```bash
# 1. 构建项目
make build

# 2. 运行程序（自动检测模式）
./build/sealien-ctrlsmt-joystick config/AMSAMOTION_IO8R.yaml

# 4. 使用默认配置运行（自动检测模式）
./build/sealien-ctrlsmt-joystick

# 5. 查看帮助信息
./build/sealien-ctrlsmt-joystick --help

# 6. 监控ROS2话题（自动启用）
ros2 topic echo /sealien_joystick  # 手柄数据
ros2 topic echo /cmd_vel           # 速度命令
ros2 topic echo /device_status     # 设备状态
```

### **手柄事件监控**
```
# 监控虚拟设备事件
sudo cat /dev/input/eventX

# 查看设备信息
ls -la /dev/input/
```

### **XBOX手柄支持**
项目已针对XBOX手柄进行了优化，特别是触发器轴的处理：

- **axis=2 (左触发器)**: 默认值-1.0，按下时变为1.0
- **axis=5 (右触发器)**: 默认值-1.0，按下时变为1.0
- **其他轴**: 默认值0.0，正常范围[-1.0, 1.0]

这确保了XBOX手柄的触发器行为符合预期，避免了初始化时的不正确值。

## 🎯 优势

### **智能手柄适配**
- 自动识别主流手柄厂商和产品型号
- 无需手动配置即可获得最佳映射效果
- 支持自定义映射配置

### **高度可定制**
- 支持每个轴和按钮的独立配置
- 提供丰富的参数调整选项
- 死区、缩放、反转等精细控制

### **性能优化**
- 高效的映射处理算法
- 最小化内存占用和CPU使用
- 实时性能监控

### **易于扩展**
- 模块化设计，便于添加新的手柄支持
- 清晰的接口定义，便于集成
- 向后兼容，不影响现有功能

## 🔄 架构重构说明

### **重构目标**
将原本集中在 `main.cpp` 中的功能函数接口分散到对应的文件夹中，使项目分层更加清晰，main函数只负责初始化和启动，避免冗余。

### **重构成果**
- **main.cpp简化**: 从587行代码减少到91行，只负责程序入口和基本流程控制
- **功能模块化**: 创建了3个专门的处理器类来管理不同的功能
- **分层清晰**: 每个模块都有明确的职责和接口
- **可维护性提升**: 各模块独立，便于维护和调试
- **可扩展性增强**: 新增功能时可以独立添加处理器类

### **新增模块**
1. **ApplicationController**: 应用程序主控制器，协调各个模块
2. **ModbusDataProcessor**: Modbus数据处理器，处理数据读取和转换
3. **JoystickManager**: USB手柄管理器，管理物理手柄检测和事件处理

### **使用方式**
重构后的程序使用方式保持不变，同时增加了新的命令行参数支持：
```bash
# 基本使用
./build/sealien-ctrlsmt-joystick [config_file] [--enable-ros2]

# 查看帮助
./build/sealien-ctrlsmt-joystick --help
```

---

*作者: SRS-HUZY*  
*版本: 1.0.0*  
*更新时间: 2025-09-08*