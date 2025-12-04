# 配置指南

## 概述

Sealien-CtrlSmt-Joystick支持灵活的配置系统，包括YAML和JSON格式的配置文件，以及智能的USB厂商配置系统。

## 配置文件类型

### 1. Modbus配置文件

位置：`config/modbus/`

#### 基本配置
```yaml
# Modbus串口配置
serial_port: auto   # 智能检测模式，自动选择最佳串口设备
# serial_port: /dev/serial/by-id/usb-FTDI_FT232R_USB_UART_BG024F00-if00-port0   # 手动指定串口（符号链接）
# serial_port: /dev/ttyUSB0   # 手动指定串口（设备节点）

# 通信参数
baud: 115200
parity: 'N'          # N=无校验, E=偶校验, O=奇校验
data_bits: 8
stop_bits: 1
slave_id: 1

# 功能开关
enable_modbus: true

# 轮询配置
poll_hz: 50.0        # 轮询频率 (Hz)
bus_error_threshold: 2  # 总线错误阈值 (秒)

# 电压转换参数
min_voltage: 0.5     # 最小电压 (V)
mid_voltage: 2.5     # 中位电压 (V)
max_voltage: 4.5     # 最大电压 (V)

# 轴映射配置
axes:
  linear_x: 1        # 线性X轴映射
  linear_y: -1        # 线性Y轴映射
  linear_z: -1        # 线性Z轴映射
  angular_x: -1       # 角速度X轴映射
  angular_y: -1       # 角速度Y轴映射
  angular_z: 0        # 角速度Z轴映射

# 按钮映射配置
buttons:
  - address: 0       # Modbus地址
    joy_button: 0    # 对应手柄按钮索引
  - address: 1
    joy_button: 1
  - address: 2
    joy_button: 2
  - address: 3
    joy_button: -1   # -1表示不映射
```

#### 智能串口检测

当`serial_port`设置为`"auto"`或`"smart"`时，系统将：

1. **自动扫描USB设备**：检测系统中的所有USB转串口设备
2. **芯片类型识别**：根据厂商ID和产品ID识别芯片类型
3. **优先级排序**：根据配置的规则选择最佳设备
4. **热插拔支持**：设备插拔时自动切换

### 2. USB厂商配置文件

位置：`config/usb/usb_vendors.yaml`

#### 厂商定义
```yaml
vendors:
  # FTDI厂商
  ftdi:
    name: "FTDI"
    description: "FTDI USB转串口芯片"
    vendor_id: "0403"
    products:
      - product_id: "6001"
        name: "FT232R"
        description: "FTDI FT232R USB转串口芯片"
        priority: 100
        chip_type: "FTDI_FT232R"
      - product_id: "6006"
        name: "FT232H"
        description: "FTDI FT232H USB转串口芯片"
        priority: 100
        chip_type: "FTDI_FT232H"

  # Silicon Labs (CP210x)厂商
  silicon_labs:
    name: "Silicon Labs"
    description: "Silicon Labs CP210x USB转串口芯片"
    vendor_id: "10c4"
    products:
      - product_id: "ea60"
        name: "CP2102"
        description: "Silicon Labs CP2102 USB转串口芯片"
        priority: 90
        chip_type: "CP210x"
```

#### 设备匹配规则
```yaml
device_rules:
  # 优先选择规则
  - name: "FTDI_Priority"
    description: "FTDI设备优先"
    vendor_id: "0403"
    priority: 20
    enabled: true
  
  - name: "CP210x_Priority"
    description: "CP210x设备优先"
    vendor_id: "10c4"
    priority: 15
    enabled: true
```

#### 设备过滤规则
```yaml
device_filters:
  - name: "Exclude_Bluetooth"
    description: "排除蓝牙设备"
    driver_pattern: "bluetooth"
    enabled: true
  
  - name: "Exclude_Modem"
    description: "排除调制解调器"
    driver_pattern: "modem"
    enabled: true
```

### 3. 手柄配置文件

位置：`config/joystick/`

#### 手柄检测配置
```yaml
# config/joystick/joystick_detector.yaml
detection:
  scan_interval_ms: 1000
  device_paths:
    - "/dev/input/js*"
  
filters:
  - name: "exclude_virtual"
    pattern: "Virtual"
    enabled: true
```

#### 手柄映射配置
```yaml
# config/joystick/joystick_mappings.yaml
mappings:
  "Logitech Gamepad F310":
    axes:
      left_stick_x: 0
      left_stick_y: 1
      right_stick_x: 2
      right_stick_y: 3
    buttons:
      a: 0
      b: 1
      x: 2
      y: 3
```

## 配置优先级

1. **命令行参数** > **配置文件** > **默认值**
2. **智能检测** > **手动指定**
3. **用户自定义规则** > **默认规则**

## 配置验证

### 检查配置语法
```bash
# 验证YAML语法
python3 -c "import yaml; yaml.safe_load(open('config/modbus/AMSAMOTION_IO8R.yaml'))"

# 验证JSON语法
python3 -c "import json; json.load(open('config.json'))"
```

### 运行时配置检查
```bash
# 启动时显示配置信息
./build/sealien-ctrlsmt-joystick config/modbus/AMSAMOTION_IO8R.yaml

# 查看USB厂商配置状态
# 程序运行时会输出配置加载信息
```

## 常见配置问题

### 1. 串口权限问题
```bash
# 添加用户到dialout组
sudo usermod -a -G dialout $USER
# 重新登录生效
```

### 2. 设备路径问题
```bash
# 查看可用串口设备
ls /dev/ttyUSB* /dev/ttyACM*

# 查看设备详细信息
udevadm info -a -p $(udevadm info -q path -n /dev/ttyUSB0)
```

### 3. 配置语法错误
- 检查YAML缩进（使用空格，不要使用Tab）
- 检查引号匹配
- 检查数据类型（数字不要加引号）

## 高级配置

### 自定义USB厂商

1. **添加新厂商**：
```yaml
vendors:
  custom_vendor:
    name: "Custom Vendor"
    vendor_id: "1234"
    products:
      - product_id: "5678"
        name: "Custom Device"
        priority: 80
        chip_type: "Custom_Device"
```

2. **添加匹配规则**：
```yaml
device_rules:
  - name: "Custom_Priority"
    vendor_id: "1234"
    priority: 10
    enabled: true
```

### 动态配置更新

系统支持运行时配置更新：

1. **修改配置文件**
2. **发送SIGHUP信号**：
```bash
killall -HUP sealien-ctrlsmt-joystick
```

### 配置模板

#### 最小配置模板
```yaml
enable_modbus: true
serial_port: auto
baud: 115200
slave_id: 1
poll_hz: 50.0
```

#### 完整配置模板
```yaml
# 基本配置
enable_modbus: true
serial_port: auto
baud: 115200
parity: 'N'
data_bits: 8
stop_bits: 1
slave_id: 1

# 轮询配置
poll_hz: 50.0
bus_error_threshold: 2

# 电压转换
min_voltage: 0.5
mid_voltage: 2.5
max_voltage: 4.5

# 轴映射
axes:
  linear_x: 1
  linear_y: -1
  linear_z: -1
  angular_x: -1
  angular_y: -1
  angular_z: 0

# 按钮映射
buttons:
  - address: 0
    joy_button: 0
  - address: 1
    joy_button: 1
  - address: 2
    joy_button: 2
```

## 配置最佳实践

1. **使用智能检测**：优先使用`serial_port: auto`
2. **合理设置优先级**：根据实际使用场景调整设备优先级
3. **配置备份**：定期备份配置文件
4. **版本控制**：将配置文件纳入版本控制
5. **测试验证**：配置修改后进行充分测试
