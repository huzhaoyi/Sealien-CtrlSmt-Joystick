# Sealien Joystick 源码编译说明

## 📋 系统要求

- **操作系统**: Linux (Ubuntu 18.04+)
- **依赖库**: 
  - libmodbus (Modbus通信)
  - yaml-cpp (配置文件解析)
  - nlohmann_json (JSON处理)
  - ROS2 Humble (可选，ROS2功能)
- **编译工具**: CMake 3.16+, GCC/Clang

## 🔧 安装依赖

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    pkg-config \
    libmodbus-dev \
    libyaml-cpp-dev \
    nlohmann-json3-dev
```

如果需要 ROS2 功能：
```bash
sudo apt install -y ros-humble-desktop
```

## 🏗️ 编译项目

### 方法一：使用 Makefile（推荐）

```bash
# 编译项目
make build

# 或者调试模式
make debug

# 或者发布模式
make release
```

### 方法二：使用 CMake 直接编译

```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

## 🚀 运行程序

```bash
# USB模式（无需配置文件）
./build/sealien-ctrlsmt-joystick-1.0.0

# Modbus模式（指定配置文件）
./build/sealien-ctrlsmt-joystick-1.0.0 config/modbus/AMSAMOTION_IO8R.yaml
```

或者使用启动脚本：
```bash
./run.sh
```

## 📦 创建部署包

```bash
./create_release_package.sh
```

## 🔍 故障排除

如果遇到依赖问题，请检查：
1. 所有依赖库是否已安装
2. CMake 版本是否 >= 3.16
3. 编译器是否支持 C++17

更多信息请参考 README.md
