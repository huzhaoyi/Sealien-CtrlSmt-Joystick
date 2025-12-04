# 📦 Web控制面板安装指南

## 🚀 快速安装

### 1. 安装Python依赖

```bash
# 进入web_control_panel目录
cd web_control_panel

# 安装Python依赖包
pip3 install -r requirements.txt
```

### 2. 安装ROS2 (如果尚未安装)

```bash
# Ubuntu/Debian系统
sudo apt update
sudo apt install ros-humble-desktop

# 设置ROS2环境
echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

### 3. 验证安装

```bash
# 检查Python依赖
python3 -c "import websockets, rclpy; print('✅ 依赖安装成功')"

# 检查ROS2环境
echo $ROS_DISTRO
```

## 📋 依赖包说明

### 核心依赖
- **websockets**: WebSocket通信库，用于实时数据传输
- **rclpy**: ROS2 Python客户端库
- **PyYAML**: YAML配置文件解析

### 系统要求
- Python 3.7+
- ROS2 Humble (推荐)
- Linux系统 (Ubuntu 22.04+ 推荐)

## 🔧 故障排除

### 常见问题

1. **websockets安装失败**
   ```bash
   pip3 install --upgrade pip
   pip3 install websockets
   ```

2. **ROS2环境未设置**
   ```bash
   source /opt/ros/humble/setup.bash
   ```

3. **权限问题**
   ```bash
   pip3 install --user -r requirements.txt
   ```

## 📞 支持

如有安装问题，请查看主项目README或联系开发团队。
