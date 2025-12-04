# 更新日志

所有重要的项目更改都将记录在此文件中。

格式基于 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)，
并且此项目遵循 [语义化版本](https://semver.org/lang/zh-CN/)。

## [未发布]

### 新增
- 添加了 WebSocket 实时通信功能
- 添加了 ROS2 话题监控
- 添加了 Modbus 设备数据监控
- 添加了实时日志系统

### 更改
- 重构了项目文件结构
- 优化了启动脚本

### 修复
- 修复了 WebSocket 连接稳定性问题

## [1.0.0] - 2025-09-19

### 新增
- 初始版本发布
- Web 控制面板基础功能
- 手柄数据实时监控
- 设备状态监控
- 配置参数可视化

### 技术特性
- HTML5 + CSS3 + JavaScript 前端
- Python WebSocket 后端
- ROS2 集成
- 响应式设计
- 实时数据更新

### 依赖
- Python 3.7+
- ROS2 Humble
- websockets >= 11.0.3
- rclpy >= 3.3.0
- PyYAML >= 6.0
