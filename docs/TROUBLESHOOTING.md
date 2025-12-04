# 故障排除

## 概述

本文档提供了Sealien-CtrlSmt-Joystick项目常见问题的解决方案和调试方法。

## 常见问题

### 1. 编译问题

#### 问题：找不到依赖库
**错误信息**：
```
CMake Error: Could not find a package configuration file provided by "yaml-cpp"
```

**解决方案**：
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install libyaml-cpp-dev libmodbus-dev

# CentOS/RHEL
sudo yum install yaml-cpp-devel libmodbus-devel

# 或者从源码编译
git clone https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp
mkdir build && cd build
cmake .. && make && sudo make install
```

#### 问题：权限不足
**错误信息**：
```
Permission denied: /dev/ttyUSB0
```

**解决方案**：
```bash
# 添加用户到dialout组
sudo usermod -a -G dialout $USER

# 重新登录或重启
sudo reboot

# 或者临时设置权限
sudo chmod 666 /dev/ttyUSB0
```

#### 问题：uinput权限问题
**错误信息**：
```
UI_ABS_SETUP failed for code=0
```

**解决方案**：
```bash
# 检查uinput模块
lsmod | grep uinput

# 如果没有加载，加载模块
sudo modprobe uinput

# 设置权限
sudo chmod 666 /dev/uinput

# 永久设置（添加到/etc/udev/rules.d/）
echo 'KERNEL=="uinput", MODE="0666"' | sudo tee /etc/udev/rules.d/99-uinput.rules
```

### 2. 设备检测问题

#### 问题：USB设备无法识别
**症状**：程序无法检测到USB转串口设备

**诊断步骤**：
```bash
# 1. 检查设备是否连接
lsusb

# 2. 检查设备文件
ls /dev/ttyUSB* /dev/ttyACM*

# 3. 检查设备信息
udevadm info -a -p $(udevadm info -q path -n /dev/ttyUSB0)

# 4. 检查驱动
cat /sys/class/tty/ttyUSB0/device/driver
```

**解决方案**：
```bash
# 1. 重新插拔设备
# 2. 检查驱动安装
sudo apt install ftdi-sio-dkms  # FTDI设备
sudo apt install ch341-dkms     # CH34x设备

# 3. 重新加载驱动
sudo modprobe -r ftdi_sio
sudo modprobe ftdi_sio

# 4. 检查udev规则
sudo udevadm control --reload-rules
sudo udevadm trigger
```

#### 问题：设备优先级不正确
**症状**：系统选择了错误的设备

**诊断步骤**：
```bash
# 查看设备信息
cat /sys/class/tty/ttyUSB0/device/idVendor
cat /sys/class/tty/ttyUSB0/device/idProduct

# 查看程序日志
./build/sealien-ctrlsmt-joystick config/modbus/AMSAMOTION_IO8R.yaml 2>&1 | grep "Smart Serial"
```

**解决方案**：
1. **更新USB厂商配置**：
```yaml
# config/usb/usb_vendors.yaml
vendors:
  your_vendor:
    name: "Your Vendor"
    vendor_id: "1234"
    products:
      - product_id: "5678"
        name: "Your Device"
        priority: 95  # 提高优先级
        chip_type: "Your_Chip"
```

2. **添加匹配规则**：
```yaml
device_rules:
  - name: "Your_Device_Priority"
    vendor_id: "1234"
    priority: 25
    enabled: true
```

#### 问题：热插拔不工作
**症状**：设备插拔时程序不响应

**诊断步骤**：
```bash
# 检查udev规则
ls /etc/udev/rules.d/

# 检查设备事件
udevadm monitor --property

# 检查程序日志
journalctl -f -u sealien-ctrlsmt-joystick
```

**解决方案**：
```bash
# 1. 创建udev规则
sudo tee /etc/udev/rules.d/99-usb-serial.rules << EOF
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", SYMLINK+="ftdi_serial"
SUBSYSTEM=="tty", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", SYMLINK+="cp2102_serial"
EOF

# 2. 重新加载规则
sudo udevadm control --reload-rules
sudo udevadm trigger

# 3. 重启程序
```

### 3. Modbus通信问题

#### 问题：Modbus连接失败
**错误信息**：
```
[MODBUS-COMM] Failed to connect to /dev/ttyUSB0
```

**诊断步骤**：
```bash
# 1. 检查串口参数
stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb

# 2. 测试串口通信
echo "test" > /dev/ttyUSB0
cat /dev/ttyUSB0

# 3. 使用modpoll测试
modpoll -m rtu -b 115200 -d 8 -s 1 -p none -t 4 -r 1 -c 1 /dev/ttyUSB0
```

**解决方案**：
1. **检查串口参数**：
```yaml
# config/modbus/AMSAMOTION_IO8R.yaml
baud: 115200
parity: 'N'
data_bits: 8
stop_bits: 1
slave_id: 1
```

2. **检查设备地址**：
```bash
# 扫描设备地址
modpoll -m rtu -b 115200 -d 8 -s 1 -p none -t 4 -r 1 -c 247 /dev/ttyUSB0
```

3. **检查线路连接**：
   - 确认A+和B-线连接正确
   - 检查终端电阻（120Ω）
   - 检查线路长度（不超过1200米）

#### 问题：Modbus数据读取失败
**错误信息**：
```
[MODBUS] Bus error detected: 3 consecutive errors
```

**诊断步骤**：
```bash
# 1. 检查设备响应
modpoll -m rtu -b 115200 -d 8 -s 1 -p none -t 4 -r 1 -c 1 /dev/ttyUSB0

# 2. 检查错误计数
# 查看程序日志中的错误信息

# 3. 检查轮询频率
# 降低poll_hz值
```

**解决方案**：
1. **调整轮询参数**：
```yaml
poll_hz: 10.0  # 降低轮询频率
bus_error_threshold: 5  # 增加错误阈值
```

2. **检查设备状态**：
```bash
# 检查设备是否正常工作
modpoll -m rtu -b 115200 -d 8 -s 1 -p none -t 4 -r 0 -c 8 /dev/ttyUSB0
```

3. **检查地址映射**：
```yaml
buttons:
  - address: 0  # 确认地址正确
    joy_button: 0
```

### 4. 手柄问题

#### 问题：手柄无法识别
**症状**：程序无法检测到USB手柄

**诊断步骤**：
```bash
# 1. 检查手柄连接
lsusb | grep -i gamepad

# 2. 检查输入设备
ls /dev/input/js*

# 3. 检查手柄信息
cat /proc/bus/input/devices | grep -A 10 -B 5 "Gamepad"

# 4. 测试手柄输入
jstest /dev/input/js0
```

**解决方案**：
```bash
# 1. 安装手柄驱动
sudo apt install joystick

# 2. 测试手柄
jstest /dev/input/js0

# 3. 检查权限
sudo chmod 666 /dev/input/js0

# 4. 添加udev规则
sudo tee /etc/udev/rules.d/99-joystick.rules << EOF
SUBSYSTEM=="input", ATTRS{name}=="*Gamepad*", MODE="0666"
EOF
```

#### 问题：手柄映射不正确
**症状**：手柄轴或按钮映射错误

**诊断步骤**：
```bash
# 1. 查看手柄详细信息
cat /proc/bus/input/devices | grep -A 20 "Your Controller"

# 2. 测试手柄输入
evtest /dev/input/event0

# 3. 查看当前映射
jstest /dev/input/js0
```

**解决方案**：
1. **更新手柄映射**：
```yaml
# config/joystick/joystick_mappings.yaml
mappings:
  "Your Controller Name":
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

2. **调整死区设置**：
```yaml
deadzone: 0.1  # 增加死区
```

### 5. ROS2集成问题

#### 问题：ROS2话题无法发布
**错误信息**：
```
[ROS2-COMM] Failed to create publisher
```

**诊断步骤**：
```bash
# 1. 检查ROS2环境
echo $ROS_DISTRO
which ros2

# 2. 检查ROS2节点
ros2 node list

# 3. 检查话题
ros2 topic list

# 4. 检查消息类型
ros2 interface show sensor_msgs/msg/Joy
```

**解决方案**：
```bash
# 1. 设置ROS2环境
source /opt/ros/humble/setup.bash

# 2. 检查ROS2安装
sudo apt install ros-humble-desktop

# 3. 重新编译
make clean && make
```

#### 问题：ROS2域ID问题
**错误信息**：
```
[ROS2] Domain ID mismatch
```

**解决方案**：
```bash
# 1. 设置域ID
export ROS_DOMAIN_ID=0

# 2. 或者使用启动脚本
./run.sh config/modbus/AMSAMOTION_IO8R.yaml 0
```

### 6. 性能问题

#### 问题：CPU使用率过高
**症状**：程序占用CPU过高

**诊断步骤**：
```bash
# 1. 查看CPU使用率
top -p $(pgrep sealien-ctrlsmt-joystick)

# 2. 查看线程状态
ps -T -p $(pgrep sealien-ctrlsmt-joystick)

# 3. 查看系统负载
htop
```

**解决方案**：
1. **降低轮询频率**：
```yaml
poll_hz: 10.0  # 降低到10Hz
```

2. **优化线程配置**：
```cpp
// 在代码中调整线程优先级
pthread_setschedparam(thread_id, SCHED_OTHER, &param);
```

#### 问题：内存泄漏
**症状**：程序内存使用持续增长

**诊断步骤**：
```bash
# 1. 监控内存使用
watch -n 1 'ps -o pid,vsz,rss,comm -p $(pgrep sealien-ctrlsmt-joystick)'

# 2. 使用valgrind检查
valgrind --leak-check=full ./build/sealien-ctrlsmt-joystick
```

**解决方案**：
1. **检查资源释放**：
```cpp
// 确保所有资源正确释放
~MyClass() {
    if (resource_) {
        delete resource_;
        resource_ = nullptr;
    }
}
```

2. **使用智能指针**：
```cpp
std::unique_ptr<Resource> resource_;
```

## 调试方法

### 1. 日志调试

#### 启用详细日志
```bash
# 编译时启用调试日志
make clean
make DEBUG=1

# 运行时启用调试
./build/sealien-ctrlsmt-joystick config/modbus/AMSAMOTION_IO8R.yaml 2>&1 | tee debug.log
```

#### 查看系统日志
```bash
# 查看系统日志
journalctl -f -u sealien-ctrlsmt-joystick

# 查看内核日志
dmesg | tail -20
```

### 2. 网络调试

#### 检查ROS2通信
```bash
# 查看ROS2节点
ros2 node list

# 查看话题
ros2 topic list

# 监听话题
ros2 topic echo /sealien_joystick

# 查看话题信息
ros2 topic info /sealien_joystick
```

### 3. 硬件调试

#### 串口调试
```bash
# 使用minicom调试串口
sudo apt install minicom
sudo minicom -D /dev/ttyUSB0 -b 115200

# 使用screen调试串口
sudo apt install screen
sudo screen /dev/ttyUSB0 115200
```

#### 手柄调试
```bash
# 使用jstest调试手柄
sudo apt install joystick
jstest /dev/input/js0

# 使用evtest调试手柄
sudo apt install evtest
evtest /dev/input/event0
```

## 性能优化

### 1. 系统优化

#### 调整内核参数
```bash
# 增加文件描述符限制
echo "* soft nofile 65536" >> /etc/security/limits.conf
echo "* hard nofile 65536" >> /etc/security/limits.conf

# 优化网络参数
echo "net.core.rmem_max = 16777216" >> /etc/sysctl.conf
echo "net.core.wmem_max = 16777216" >> /etc/sysctl.conf
```

#### 调整进程优先级
```bash
# 设置进程优先级
sudo nice -n -10 ./build/sealien-ctrlsmt-joystick

# 设置实时优先级
sudo chrt -f 50 ./build/sealien-ctrlsmt-joystick
```

### 2. 应用优化

#### 优化轮询频率
```yaml
# 根据实际需求调整
poll_hz: 50.0  # 50Hz适合大多数应用
# poll_hz: 100.0  # 100Hz适合高精度应用
# poll_hz: 10.0   # 10Hz适合低功耗应用
```

#### 优化缓冲区大小
```cpp
// 在代码中调整缓冲区大小
constexpr size_t BUFFER_SIZE = 1024;
```

## 联系支持

如果问题仍然存在，请联系技术支持：

- **邮箱**: hu418@163.com
- **项目地址**: [Sealien-CtrlSmt-Joystick](https://github.com/your-repo/sealien-ctrlsmt-joystick)

请提供以下信息：
1. 系统信息（操作系统版本、内核版本）
2. 错误日志
3. 配置文件内容
4. 设备信息（lsusb、udevadm info等）
5. 复现步骤
