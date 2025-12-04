# Modbus模式持久性修复

## 问题描述

用户反馈USB转串口设备断开后，系统立即切换到USB手柄模式，这导致自动重连功能失去意义。系统应该保持Modbus模式并等待设备重新连接。

## 问题分析

在`src/core/application_controller.cpp`的模式切换逻辑中，原来的代码是：

```cpp
// 模式切换逻辑：如果modbus未启用，则只使用USB模式
std::string new_mode = "standby";
if (enable_modbus_ && current_modbus_state) {
    new_mode = "modbus";
} else if (current_usb_count > 0) {
    new_mode = "usb";
}
```

这个逻辑的问题是：
- 只有当`current_modbus_state`为true时才使用modbus模式
- 当Modbus设备断开连接时（`current_modbus_state`为false），立即切换到USB模式
- 这导致自动重连功能无法正常工作，因为系统已经切换到USB模式了

## 解决方案

修改模式切换逻辑，当Modbus启用时，始终使用Modbus模式，即使设备暂时断开连接也保持Modbus模式等待重连：

```cpp
// 模式切换逻辑：如果modbus启用，优先使用modbus模式（即使暂时断开也保持modbus模式等待重连）
std::string new_mode = "standby";
if (enable_modbus_) {
    new_mode = "modbus";  // 启用modbus时始终使用modbus模式，支持自动重连
} else if (current_usb_count > 0) {
    new_mode = "usb";
}
```

## 修改效果

### 修改前
1. USB转串口设备断开 → 系统检测到Modbus断开 → 立即切换到USB手柄模式
2. 设备重新连接 → 系统仍在USB模式 → 自动重连功能无法发挥作用

### 修改后
1. USB转串口设备断开 → 系统保持Modbus模式 → 继续尝试自动重连
2. 设备重新连接 → 系统仍在Modbus模式 → 自动重连成功 → 恢复正常工作

## 预期行为

现在当USB转串口设备断开时，系统会：

1. **保持Modbus模式**：不会切换到USB手柄模式
2. **继续尝试重连**：Modbus客户端会持续尝试重新连接
3. **等待设备恢复**：USB转串口设备检测器会监控设备重新连接
4. **自动恢复工作**：设备重新连接后，Modbus通信自动恢复

## 日志输出变化

### 修改前的日志
```
[MODBUS-COMM] Device does not exist, cannot reconnect
[CORE] Mode switching: modbus -> usb
[CORE] Cleaning up mode: modbus
[CORE] Initializing mode: usb
```

### 修改后的预期日志
```
[MODBUS-COMM] Device does not exist, cannot reconnect
[MODBUS-COMM] Attempting to reconnect to /dev/ttyUSB0
[CORE] USB Serial device detected: /dev/ttyUSB0
[MODBUS-COMM] === RECONNECT SUCCESS ===
[MODBUS-COMM] Successfully reconnected to /dev/ttyUSB0
```

## 注意事项

1. **模式优先级**：当Modbus启用时，系统始终优先使用Modbus模式
2. **自动重连**：只有在Modbus模式下，自动重连功能才能正常工作
3. **USB手柄**：只有在Modbus未启用时，系统才会使用USB手柄模式
4. **向后兼容**：如果用户禁用Modbus，系统仍然会正常切换到USB手柄模式

## 测试建议

1. **断开测试**：断开USB转串口设备，观察系统是否保持Modbus模式
2. **重连测试**：重新连接设备，观察自动重连是否成功
3. **模式切换测试**：禁用Modbus配置，观察系统是否正常切换到USB模式
4. **长时间测试**：让设备断开较长时间，观察系统是否持续尝试重连

这个修改确保了USB转串口设备的自动重连功能能够正常工作，提高了系统的可靠性和用户体验。
