# 设备支持

## 概述

Sealien-CtrlSmt-Joystick支持多种USB转串口芯片和手柄设备，通过智能检测和配置系统实现即插即用功能。

## USB转串口芯片支持

### 支持的厂商和芯片

#### 1. FTDI (Future Technology Devices International)
- **厂商ID**: `0403`
- **优先级**: 100 (最高)
- **支持芯片**:
  - FT232R (产品ID: 6001) - 经典单通道USB转串口
  - FT232H (产品ID: 6006) - 高速USB转串口
  - FT2232H (产品ID: 6010) - 双通道USB转串口
  - FT4232H (产品ID: 6011) - 四通道USB转串口
  - FT2232C (产品ID: 6014) - 双通道USB转串口
  - FT2232D (产品ID: 6015) - 双通道USB转串口

#### 2. Silicon Labs (CP210x系列)
- **厂商ID**: `10c4`
- **优先级**: 90 (高)
- **支持芯片**:
  - CP2102 (产品ID: ea60) - 单通道USB转串口
  - CP2103 (产品ID: ea61) - 单通道USB转串口
  - CP2104 (产品ID: ea70) - 单通道USB转串口
  - CP2105 (产品ID: ea71) - 单通道USB转串口
  - CP2108 (产品ID: ea80) - 多通道USB转串口
  - CP2109 (产品ID: ea81) - 多通道USB转串口

#### 3. 沁恒 (CH34x系列)
- **厂商ID**: `1a86`
- **优先级**: 80 (中等)
- **支持芯片**:
  - CH340 (产品ID: 7523) - 单通道USB转串口
  - CH341 (产品ID: 5523) - 单通道USB转串口

#### 4. Prolific (PL2303)
- **厂商ID**: `067b`
- **优先级**: 60 (较低)
- **支持芯片**:
  - PL2303 (产品ID: 2303) - 单通道USB转串口

#### 5. CDC ACM设备
- **厂商ID**: 任意
- **优先级**: 70 (中等)
- **识别方式**: 驱动名称匹配 `cdc_acm`
- **说明**: 通用USB转串口设备，如Arduino、STM32等

### 设备优先级说明

系统根据以下因素计算设备优先级：

1. **芯片类型基础优先级**:
   - FTDI: 100分
   - CP210x: 90分
   - CH34x: 80分
   - CDC ACM: 70分
   - PL2303: 60分
   - 其他: 50分

2. **匹配规则加分**:
   - 用户自定义规则优先级
   - 设备特征匹配加分

3. **过滤规则**:
   - 排除蓝牙设备
   - 排除调制解调器
   - 排除虚拟设备

## 手柄设备支持

### 支持的USB手柄

#### 1. 通用手柄
- **Xbox 360手柄**: Microsoft Xbox 360 Controller
- **Xbox One手柄**: Microsoft Xbox One Controller
- **PlayStation手柄**: Sony DualShock 3/4
- **Logitech手柄**: Logitech Gamepad F310/F710
- **其他标准手柄**: 符合HID标准的USB手柄

#### 2. 手柄特性
- **轴数量**: 支持最多8个模拟轴
- **按钮数量**: 支持最多32个数字按钮
- **轮询频率**: 支持最高1000Hz轮询
- **死区处理**: 可配置的死区范围

### 手柄映射支持

#### 1. 自动映射
系统自动识别常见手柄的轴和按钮布局：

```yaml
# Xbox 360手柄映射
"Microsoft X-Box 360 pad":
  axes:
    left_stick_x: 0
    left_stick_y: 1
    right_stick_x: 2
    right_stick_y: 3
    left_trigger: 4
    right_trigger: 5
  buttons:
    a: 0
    b: 1
    x: 2
    y: 3
    left_bumper: 4
    right_bumper: 5
    back: 6
    start: 7
    left_stick: 8
    right_stick: 9
```

#### 2. 自定义映射
支持用户自定义手柄映射：

```yaml
mappings:
  "Custom Controller":
    axes:
      custom_axis_1: 0
      custom_axis_2: 1
    buttons:
      custom_button_1: 0
      custom_button_2: 1
```

#### 3. 轴映射类型
| 类型 | 值 | 说明 | 用途 |
|------|----|----|----|
| `LINEAR_X` | 0 | 线速度X轴 | 控制机器人前进/后退 |
| `LINEAR_Y` | 1 | 线速度Y轴 | 控制机器人左右移动 |
| `LINEAR_Z` | 2 | 线速度Z轴 | 控制机器人上升/下降 |
| `ANGULAR_X` | 3 | 角速度X轴 | 控制机器人俯仰 |
| `ANGULAR_Y` | 4 | 角速度Y轴 | 控制机器人横滚 |
| `ANGULAR_Z` | 5 | 角速度Z轴 | 控制机器人转向 |

#### 4. 按钮功能类型
| 类型 | 值 | 说明 | 默认行为 |
|------|----|----|----|
| `EMERGENCY_STOP` | 0 | 紧急停止 | 按下时停止所有运动 |
| `MODE_SWITCH` | 1 | 模式切换 | 切换慢速/正常速度模式 |
| `SPEED_BOOST` | 2 | 速度提升 | 按下时提升速度 |
| `SPEED_SLOW` | 3 | 速度降低 | 按下时降低速度 |

#### 5. 高级映射配置
```yaml
joystick_mappings:
  - vendor_id: 0x045E
    product_id: 0x02D1
    device_name: "Xbox One Controller"
    config_name: "Xbox_One_Default"
    default_deadzone: 0.12
    default_scale_factor: 1.0
    enable_auto_detection: true
    
    axis_mappings:
      - axis_index: 0
        mapping_type: LINEAR_X
        deadzone: 0.1
        scale_factor: 1.0
        invert: false
        
    button_mappings:
      - button_index: 0
        function_type: EMERGENCY_STOP
        press_action: "stop_all_movement"
        release_action: "resume_movement"
```

## Modbus设备支持

### 支持的Modbus设备

#### 1. AMSAMOTION IO8R
- **设备类型**: 8路数字输入/输出模块
- **协议**: Modbus RTU
- **地址范围**: 0-7 (数字输入/输出)
- **电压范围**: 0-5V
- **通信参数**: 115200 baud, 8N1
- **分辨率**: 12位 (0-4095)

#### 2. 通用Modbus设备
- **支持功能码**:
  - 01: 读线圈状态
  - 02: 读离散输入状态
  - 03: 读保持寄存器
  - 04: 读输入寄存器
  - 05: 写单个线圈
  - 06: 写单个寄存器
  - 15: 写多个线圈
  - 16: 写多个寄存器

### Modbus协议规格

#### 1. 协议帧格式
```
[站号] [功能码] [数据长度] [数据] [CRC校验码]
```

#### 2. 模拟量通道映射
| 通道 | 寄存器地址 | 数据长度 | 信号类型 | 描述 |
|------|------------|----------|----------|------|
| V1   | 0x0000     | 2字节    | 0~10V电压 | 第一路模拟量 0~10V 电压输入 |
| V2   | 0x0001     | 2字节    | 0~10V电压 | 第二路模拟量 0~10V 电压输入 |
| V3   | 0x0002     | 2字节    | 0~10V电压 | 第三路模拟量 0~10V 电压输入 |
| I1   | 0x0003     | 2字节    | 0~20mA电流 | 第一路模拟量 0~20mA 电流输入 |
| I2   | 0x0004     | 2字节    | 0~20mA电流 | 第二路模拟量 0~20mA 电流输入 |
| I3   | 0x0005     | 2字节    | 0~20mA电流 | 第三路模拟量 0~20mA 电流输入 |

#### 3. 数字量通道映射
| 通道 | 地址 | 类型 | 描述 |
|------|------|------|------|
| DI1  | 0    | 输入 | 第一路数字输入 |
| DI2  | 1    | 输入 | 第二路数字输入 |
| DI3  | 2    | 输入 | 第三路数字输入 |
| DI4  | 3    | 输入 | 第四路数字输入 |
| DI5  | 4    | 输入 | 第五路数字输入 |
| DI6  | 5    | 输入 | 第六路数字输入 |
| DI7  | 6    | 输入 | 第七路数字输入 |
| DI8  | 7    | 输入 | 第八路数字输入 |
| DO1  | 0    | 输出 | 第一路数字输出 |
| DO2  | 1    | 输出 | 第二路数字输出 |
| DO3  | 2    | 输出 | 第三路数字输出 |
| DO4  | 3    | 输出 | 第四路数字输出 |
| DO5  | 4    | 输出 | 第五路数字输出 |
| DO6  | 5    | 输出 | 第六路数字输出 |
| DO7  | 6    | 输出 | 第七路数字输出 |
| DO8  | 7    | 输出 | 第八路数字输出 |

### Modbus配置示例

```yaml
# AMSAMOTION IO8R配置
modbus:
  device_type: "AMSAMOTION_IO8R"
  slave_id: 1
  registers:
    digital_inputs:
      start_address: 0
      count: 8
    digital_outputs:
      start_address: 0
      count: 8
  voltage_conversion:
    min_voltage: 0.5
    max_voltage: 4.5
    resolution: 4096
```

## 设备检测和识别

### 1. USB设备检测

系统通过以下方式检测USB设备：

```bash
# 扫描USB设备
lsusb

# 查看串口设备
ls /dev/ttyUSB* /dev/ttyACM*

# 查看设备详细信息
udevadm info -a -p $(udevadm info -q path -n /dev/ttyUSB0)
```

### 2. 设备信息读取

从sysfs读取设备信息：

```bash
# 厂商ID
cat /sys/class/tty/ttyUSB0/device/idVendor

# 产品ID
cat /sys/class/tty/ttyUSB0/device/idProduct

# 序列号
cat /sys/class/tty/ttyUSB0/device/serial

# 驱动名称
cat /sys/class/tty/ttyUSB0/device/driver
```

### 3. 设备状态监控

系统实时监控设备状态：

- **连接检测**: 定期检查设备文件是否存在
- **通信测试**: 尝试打开设备进行通信测试
- **状态回调**: 设备状态变化时触发回调

## 添加新设备支持

### 1. 添加新的USB转串口芯片

1. **更新厂商配置** (`config/usb/usb_vendors.yaml`):
```yaml
vendors:
  new_vendor:
    name: "New Vendor"
    vendor_id: "1234"
    products:
      - product_id: "5678"
        name: "New Chip"
        priority: 85
        chip_type: "New_Chip"
```

2. **添加匹配规则**:
```yaml
device_rules:
  - name: "New_Chip_Priority"
    vendor_id: "1234"
    priority: 15
    enabled: true
```

### 2. 添加新的手柄支持

1. **更新手柄映射** (`config/joystick/joystick_mappings.yaml`):
```yaml
mappings:
  "New Controller":
    axes:
      axis_1: 0
      axis_2: 1
    buttons:
      button_1: 0
      button_2: 1
```

2. **测试手柄识别**:
```bash
# 查看手柄信息
cat /proc/bus/input/devices

# 测试手柄输入
jstest /dev/input/js0
```

### 3. 添加新的Modbus设备

1. **创建设备配置**:
```yaml
modbus:
  device_type: "New_Device"
  slave_id: 1
  registers:
    custom_register:
      start_address: 100
      count: 10
```

2. **实现数据处理**:
```cpp
// 在modbus_data_processor.cpp中添加处理逻辑
void ModbusDataProcessor::processNewDevice() {
    // 处理新设备的数据
}
```

## 设备兼容性测试

### 1. USB转串口测试

```bash
# 测试串口通信
echo "test" > /dev/ttyUSB0
cat /dev/ttyUSB0

# 测试Modbus通信
modpoll -m rtu -b 115200 -d 8 -s 1 -p none -t 4 -r 1 -c 1 /dev/ttyUSB0
```

### 2. 手柄测试

```bash
# 测试手柄输入
jstest /dev/input/js0

# 测试手柄事件
evtest /dev/input/event0
```

### 3. 系统集成测试

```bash
# 启动程序测试
./build/sealien-ctrlsmt-joystick config/modbus/AMSAMOTION_IO8R.yaml

# 检查日志输出
journalctl -f -u sealien-ctrlsmt-joystick
```

## 故障排除

### 1. 设备识别问题

**问题**: 设备无法识别
**解决方案**:
- 检查设备连接
- 检查驱动安装
- 检查权限设置
- 查看系统日志

### 2. 通信问题

**问题**: Modbus通信失败
**解决方案**:
- 检查串口参数
- 检查设备地址
- 检查线路连接
- 检查终端电阻

### 3. 映射问题

**问题**: 手柄映射不正确
**解决方案**:
- 检查手柄类型
- 更新映射配置
- 测试手柄输入
- 调整死区设置
