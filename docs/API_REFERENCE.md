# API参考

## 概述

本文档提供了Sealien-CtrlSmt-Joystick项目的API参考，包括核心类、接口和数据结构。

## 核心模块

### 1. 应用控制器 (ApplicationController)

#### 类定义
```cpp
class ApplicationController {
public:
    ApplicationController();
    ~ApplicationController();
    
    bool initialize(const std::string& config_path, bool enable_ros2 = true);
    void run();
    void stop();
    void cleanup();
    
private:
    bool initializeConfig(const std::string& config_path);
    bool initializeModbus();
    bool initializeJoystick();
    bool initializeROS2();
    void initializeUInput();
};
```

#### 主要方法

##### `initialize(const std::string& config_path, bool enable_ros2)`
初始化应用程序控制器。

**参数**:
- `config_path`: 配置文件路径
- `enable_ros2`: 是否启用ROS2功能

**返回值**: 成功返回true，失败返回false

**示例**:
```cpp
ApplicationController controller;
if (controller.initialize("config/modbus/AMSAMOTION_IO8R.yaml")) {
    controller.run();
}
```

##### `run()`
启动应用程序主循环。

**示例**:
```cpp
controller.run();
```

##### `stop()`
停止应用程序。

**示例**:
```cpp
controller.stop();
```

### 2. 智能配置管理器 (SmartConfigManager)

#### 类定义
```cpp
class SmartConfigManager {
public:
    SmartConfigManager();
    ~SmartConfigManager();
    
    bool loadConfig(const std::string& config_path, bool enable_smart_detection = true);
    bool setConfig(const Config& config);
    
    const Config& getConfig() const;
    std::string getCurrentSerialPort() const;
    bool isSmartDetectionEnabled() const;
    
    void enableSmartDetection(bool enable);
    void setSerialPortCallback(SerialPortCallback callback);
    
    std::string getSerialPortStatus() const;
    void resetToDefault();
};
```

#### 主要方法

##### `loadConfig(const std::string& config_path, bool enable_smart_detection)`
加载配置文件。

**参数**:
- `config_path`: 配置文件路径
- `enable_smart_detection`: 是否启用智能检测

**返回值**: 成功返回true，失败返回false

**示例**:
```cpp
SmartConfigManager config_manager;
if (config_manager.loadConfig("config/modbus/AMSAMOTION_IO8R.yaml")) {
    const auto& config = config_manager.getConfig();
    std::cout << "Serial port: " << config.getSerialPort() << std::endl;
}
```

##### `setSerialPortCallback(SerialPortCallback callback)`
设置串口变化回调。

**参数**:
- `callback`: 回调函数

**示例**:
```cpp
config_manager.setSerialPortCallback([](const std::string& old_port, const std::string& new_port) {
    std::cout << "Serial port changed: " << old_port << " -> " << new_port << std::endl;
});
```

### 3. 智能串口检测器 (SmartSerialDetector)

#### 类定义
```cpp
class SmartSerialDetector {
public:
    SmartSerialDetector();
    ~SmartSerialDetector();
    
    bool start(bool enable_hotplug = true);
    void stop();
    
    std::vector<std::string> getAvailableDevices() const;
    std::string selectBestDevice(const std::string& preferred_device = "") const;
    
    void setDeviceCallback(DeviceCallback callback);
    void setVendorConfigPath(const std::string& config_path);
    
    std::string getVendorConfigStatus() const;
    void setDefaultDeviceRules();
};
```

#### 主要方法

##### `start(bool enable_hotplug)`
启动设备检测器。

**参数**:
- `enable_hotplug`: 是否启用热插拔监控

**返回值**: 成功返回true，失败返回false

**示例**:
```cpp
SmartSerialDetector detector;
if (detector.start(true)) {
    auto devices = detector.getAvailableDevices();
    for (const auto& device : devices) {
        std::cout << "Found device: " << device << std::endl;
    }
}
```

##### `selectBestDevice(const std::string& preferred_device)`
选择最佳设备。

**参数**:
- `preferred_device`: 首选设备路径

**返回值**: 最佳设备路径

**示例**:
```cpp
std::string best_device = detector.selectBestDevice("/dev/ttyUSB0");
std::cout << "Best device: " << best_device << std::endl;
```

### 4. USB厂商配置 (USBVendorConfig)

#### 类定义
```cpp
class USBVendorConfig {
public:
    USBVendorConfig();
    ~USBVendorConfig();
    
    bool loadConfig(const std::string& config_path);
    
    const USBVendorInfo* getVendorInfo(const std::string& vendor_id) const;
    const USBProductInfo* getProductInfo(const std::string& vendor_id, const std::string& product_id) const;
    
    std::string identifyChipType(const std::string& vendor_id, 
                                const std::string& product_id, 
                                const std::string& driver_name) const;
    
    int calculateDevicePriority(const std::string& vendor_id, 
                               const std::string& product_id, 
                               const std::string& driver_name) const;
    
    bool shouldFilterDevice(const std::string& driver_name) const;
    
    void addVendorInfo(const USBVendorInfo& vendor_info);
    void addMatchRule(const DeviceMatchRule& rule);
    void addFilterRule(const DeviceFilterRule& rule);
    
    std::string getConfigStatus() const;
    void resetToDefault();
};
```

#### 主要方法

##### `loadConfig(const std::string& config_path)`
加载USB厂商配置。

**参数**:
- `config_path`: 配置文件路径

**返回值**: 成功返回true，失败返回false

**示例**:
```cpp
USBVendorConfig vendor_config;
if (vendor_config.loadConfig("config/usb/usb_vendors.yaml")) {
    std::string chip_type = vendor_config.identifyChipType("0403", "6001", "ftdi_sio");
    std::cout << "Chip type: " << chip_type << std::endl;
}
```

##### `identifyChipType(const std::string& vendor_id, const std::string& product_id, const std::string& driver_name)`
识别芯片类型。

**参数**:
- `vendor_id`: 厂商ID
- `product_id`: 产品ID
- `driver_name`: 驱动名称

**返回值**: 芯片类型字符串

### 5. Modbus客户端 (ModbusClient)

#### 类定义
```cpp
class ModbusClient {
public:
    ModbusClient(const std::string& port, int baud, char parity, int data_bits, int stop_bits);
    ~ModbusClient();
    
    bool connect();
    void disconnect();
    bool isConnected() const;
    
    bool readHoldingRegisters(int slave_id, int start_addr, int count, uint16_t* data);
    bool writeSingleRegister(int slave_id, int addr, uint16_t value);
    bool writeMultipleRegisters(int slave_id, int start_addr, int count, const uint16_t* data);
    
    void enableAutoReconnect(bool enable, int interval_ms = 5000, int max_attempts = 10);
    void setConnectionCallback(ConnectionCallback callback);
    
    std::string getLastError() const;
    int getBusErrorCount() const;
    void resetBusErrorCount();
};
```

#### 主要方法

##### `connect()`
连接到Modbus设备。

**返回值**: 成功返回true，失败返回false

**示例**:
```cpp
ModbusClient client("/dev/ttyUSB0", 115200, 'N', 8, 1);
if (client.connect()) {
    uint16_t data[8];
    if (client.readHoldingRegisters(1, 0, 8, data)) {
        std::cout << "Read successful" << std::endl;
    }
}
```

##### `readHoldingRegisters(int slave_id, int start_addr, int count, uint16_t* data)`
读取保持寄存器。

**参数**:
- `slave_id`: 从站ID
- `start_addr`: 起始地址
- `count`: 寄存器数量
- `data`: 数据缓冲区

**返回值**: 成功返回true，失败返回false

### 6. 手柄管理器 (JoystickManager)

#### 类定义
```cpp
class JoystickManager {
public:
    JoystickManager();
    ~JoystickManager();
    
    bool initialize();
    void start();
    void stop();
    void pause();
    void resume();
    
    void setDataCallback(JoystickDataCallback callback);
    void setDeviceCallback(JoystickDeviceCallback callback);
    
    std::vector<JoystickDevice> getConnectedDevices() const;
    bool isDeviceConnected(const std::string& device_path) const;
    
    void setAxisMapping(const AxisMapping& mapping);
    void setButtonMapping(const ButtonMapping& mapping);
    
    void setDeadzone(float deadzone);
    void setFilterEnabled(bool enabled);
};
```

#### 主要方法

##### `initialize()`
初始化手柄管理器。

**返回值**: 成功返回true，失败返回false

**示例**:
```cpp
JoystickManager joystick_manager;
if (joystick_manager.initialize()) {
    joystick_manager.setDataCallback([](const JoystickData& data) {
        std::cout << "Axis 0: " << data.axes[0] << std::endl;
    });
    joystick_manager.start();
}
```

##### `setDataCallback(JoystickDataCallback callback)`
设置数据回调。

**参数**:
- `callback`: 数据回调函数

## 数据结构

### 1. 配置结构 (Config)

```cpp
struct Config {
    // 串口配置
    std::string serial_port;
    int baud;
    char parity;
    int data_bits;
    int stop_bits;
    int slave_id;
    
    // 功能开关
    bool enable_modbus;
    
    // 轮询配置
    double poll_hz;
    int bus_error_threshold;
    
    // 电压转换
    double min_voltage;
    double mid_voltage;
    double max_voltage;
    
    // 轴映射
    AxisMapping axes;
    
    // 按钮映射
    std::vector<ButtonMapping> buttons;
    
    // 访问方法
    std::string getSerialPort() const;
    int getBaud() const;
    char getParity() const;
    int getDataBits() const;
    int getStopBits() const;
    int getSlaveId() const;
    bool getEnableModbus() const;
    double getPollHz() const;
    int getBusErrorThreshold() const;
    double getMinVoltage() const;
    double getMidVoltage() const;
    double getMaxVoltage() const;
    const AxisMapping& getAxes() const;
    const std::vector<ButtonMapping>& getButtons() const;
};
```

### 2. USB设备信息 (SmartSerialDeviceInfo)

```cpp
struct SmartSerialDeviceInfo {
    std::string device_path;      // 设备路径
    std::string device_name;      // 设备名称
    std::string vendor_id;       // 厂商ID
    std::string product_id;      // 产品ID
    std::string serial_number;   // 序列号
    std::string driver_name;     // 驱动名称
    std::string chip_type;       // 芯片类型
    std::string usb_path;        // USB路径
    
    bool is_connected;           // 连接状态
    int priority;               // 优先级
    int reconnect_attempts;     // 重连尝试次数
    
    std::chrono::steady_clock::time_point last_seen;
    std::chrono::steady_clock::time_point last_reconnect_attempt;
};
```

### 3. 手柄数据 (JoystickData)

```cpp
struct JoystickData {
    std::vector<float> axes;     // 轴数据
    std::vector<bool> buttons;   // 按钮数据
    std::string device_path;     // 设备路径
    std::chrono::steady_clock::time_point timestamp;
};
```

### 4. 轴映射 (AxisMapping)

```cpp
struct AxisMapping {
    int linear_x;    // 线性X轴
    int linear_y;    // 线性Y轴
    int linear_z;    // 线性Z轴
    int angular_x;   // 角速度X轴
    int angular_y;   // 角速度Y轴
    int angular_z;   // 角速度Z轴
};
```

### 5. 按钮映射 (ButtonMapping)

```cpp
struct ButtonMapping {
    int address;        // Modbus地址
    int joy_button;     // 手柄按钮索引
};
```

## 回调函数类型

### 1. 设备回调

```cpp
using DeviceCallback = std::function<void(const std::string& device_path, bool connected)>;
```

### 2. 串口变化回调

```cpp
using SerialPortCallback = std::function<void(const std::string& old_port, const std::string& new_port)>;
```

### 3. 连接状态回调

```cpp
using ConnectionCallback = std::function<void(bool connected)>;
```

### 4. 手柄数据回调

```cpp
using JoystickDataCallback = std::function<void(const JoystickData& data)>;
```

### 5. 手柄设备回调

```cpp
using JoystickDeviceCallback = std::function<void(const std::string& device_path, bool connected)>;
```

## 错误处理

### 1. 异常类型

```cpp
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class DeviceException : public std::runtime_error {
public:
    DeviceException(const std::string& message) : std::runtime_error(message) {}
};

class ModbusException : public std::runtime_error {
public:
    ModbusException(const std::string& message) : std::runtime_error(message) {}
};
```

### 2. 错误码

```cpp
enum class ErrorCode {
    SUCCESS = 0,
    CONFIG_LOAD_FAILED,
    DEVICE_NOT_FOUND,
    MODBUS_CONNECTION_FAILED,
    PERMISSION_DENIED,
    INVALID_PARAMETER
};
```

## 使用示例

### 1. 基本使用

```cpp
#include "application_controller.h"

int main() {
    ApplicationController controller;
    
    if (!controller.initialize("config/modbus/AMSAMOTION_IO8R.yaml")) {
        std::cerr << "Failed to initialize controller" << std::endl;
        return -1;
    }
    
    controller.run();
    controller.cleanup();
    
    return 0;
}
```

### 2. 自定义设备检测

```cpp
#include "smart_serial_detector.h"

int main() {
    SmartSerialDetector detector;
    
    detector.setVendorConfigPath("config/usb/usb_vendors.yaml");
    detector.setDeviceCallback([](const std::string& device, bool connected) {
        std::cout << "Device " << device << " " << (connected ? "connected" : "disconnected") << std::endl;
    });
    
    if (detector.start(true)) {
        auto devices = detector.getAvailableDevices();
        std::string best = detector.selectBestDevice();
        std::cout << "Best device: " << best << std::endl;
    }
    
    return 0;
}
```

### 3. Modbus通信

```cpp
#include "modbus_client.h"

int main() {
    ModbusClient client("/dev/ttyUSB0", 115200, 'N', 8, 1);
    
    if (client.connect()) {
        uint16_t data[8];
        if (client.readHoldingRegisters(1, 0, 8, data)) {
            for (int i = 0; i < 8; i++) {
                std::cout << "Register " << i << ": " << data[i] << std::endl;
            }
        }
    }
    
    return 0;
}
```

## 编译和链接

### 1. CMake配置

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMODBUS REQUIRED libmodbus)
find_package(yaml-cpp REQUIRED)

target_link_libraries(sealien-ctrlsmt-joystick
    ${LIBMODBUS_LIBRARIES}
    yaml-cpp
    pthread
)
```

### 2. 包含路径

```cpp
#include "application_controller.h"
#include "smart_config.h"
#include "smart_serial_detector.h"
#include "modbus_client.h"
#include "joystick_manager.h"
```
