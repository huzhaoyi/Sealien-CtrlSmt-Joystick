# 🎮 Sealien 手柄转换器 - Web控制面板

## 📋 简介

这是一个现代化的Web控制面板，用于实时监控Sealien手柄转换器的各种数据，包括手柄输入、Modbus设备数据、ROS2话题和系统状态。

## ✨ 主要功能

- 🎮 **实时手柄数据监控** - 12个轴（4个摇杆+8个旋钮）和32个按钮的实时状态
- 📡 **Modbus设备数据** - 8个模拟量输入通道电压监控
- 🤖 **ROS2话题监控** - 实时显示ROS2话题数据
- 📊 **系统状态监控** - USB手柄、Modbus设备、ROS2节点状态
- 📋 **实时日志系统** - 系统运行日志实时显示
- ⚙️ **配置信息显示** - 串口、电压映射、轴配置等参数

## 🚀 快速开始

### 安装依赖

在启动Web控制面板之前，需要安装必要的依赖：

```bash
# 进入web_control_panel目录
cd web_control_panel

# 安装Python依赖
pip3 install -r requirements.txt

# 安装ROS2 (如果尚未安装)
sudo apt install ros-humble-desktop

# 设置ROS2环境
source /opt/ros/humble/setup.bash
```

**详细安装指南**: 请参考 [INSTALL.md](INSTALL.md)

### 启动Web控制面板

```bash
# 进入web_control_panel目录
cd web_control_panel

# 启动Web控制面板
./scripts/start.sh
```

**注意**：使用ROS2数据源需要：
1. 安装ROS2：`sudo apt install ros-humble-desktop`
2. 设置ROS2环境：`source /opt/ros/humble/setup.bash`
3. 安装Python依赖：`pip3 install -r requirements.txt`
4. 先运行手柄程序：`./run.sh`（在项目根目录）

### 访问Web界面

启动服务器后，打开浏览器访问：

```
http://localhost:8080/frontend/web_dashboard.html
```

## 📁 文件结构

```
web_control_panel/
├── 📁 frontend/               # 前端文件
│   ├── web_dashboard.html     # Web界面主文件
│   ├── favicon.svg            # 网站图标
│   ├── 📁 css/                # CSS样式文件
│   ├── 📁 js/                 # JavaScript文件
│   └── 📁 assets/             # 前端资源
│       └── 📁 fonts/          # 字体文件
├── 📁 backend/                # 后端服务
│   └── ros2_websocket_server.py # ROS2 WebSocket服务器
├── 📁 assets/                 # 静态资源
│   └── 📁 images/             # 图片资源
│       └── SEALIEN-LOGO.png   # 公司Logo
├── 📁 config/                 # 配置文件
│   └── config.yaml.example    # 配置文件示例
├── 📁 scripts/                # 脚本文件
│   └── start.sh               # 启动脚本
├── 📁 docs/                   # 文档
│   ├── README.md              # 详细说明文档
│   ├── INSTALL.md             # 安装指南
│   └── API.md                 # API文档
├── 📁 tests/                  # 测试文件
│   └── test_websocket_server.py # WebSocket服务器测试
├── 📁 logs/                   # 日志文件目录
├── requirements.txt            # Python依赖包列表
├── Dockerfile                  # Docker容器配置
├── docker-compose.yml          # Docker编排配置
├── .gitignore                  # Git忽略文件
├── LICENSE                     # 开源许可证
├── CHANGELOG.md                # 版本更新日志
└── README.md                   # 本文件
```

## 🛠️ 技术架构

### 前端技术
- **HTML5**: 现代化语义化结构
- **CSS3**: 响应式设计和动画效果
- **JavaScript**: 原生ES6+代码
- **WebSocket**: 实时双向通信

### 后端技术
- **Python3**: WebSocket服务器
- **asyncio**: 异步编程支持
- **websockets**: WebSocket协议实现

### 数据更新频率
- **手柄数据**: 100ms
- **Modbus数据**: 100ms
- **系统状态**: 100ms
- **日志更新**: 2秒

## 🎨 界面说明

### 状态栏
- **系统状态**: 显示整体运行状态
- **USB手柄**: 显示手柄连接状态
- **Modbus设备**: 显示Modbus设备状态
- **ROS2节点**: 显示ROS2节点状态
- **轮询频率**: 显示数据更新频率
- **运行时间**: 显示系统运行时长

### 手柄数据面板
- **轴显示**: 12个轴的实时数值和条形图（前4个为摇杆，后8个为旋钮）
- **按钮显示**: 32个按钮的按下/释放状态
- **颜色编码**: 绿色表示按下，灰色表示释放

### Modbus数据面板
- **电压通道**: 8个模拟量输入通道的电压值
- **离散输入**: 数字输入的状态显示

### ROS2话题面板
- **话题状态**: 显示话题是否活跃
- **消息数据**: JSON格式显示最新消息

### 日志面板
- **实时日志**: 显示系统运行日志
- **日志级别**: 不同颜色表示不同级别

### 配置面板
- **串口配置**: Modbus串口参数
- **电压映射**: 电压范围设置
- **轴配置**: 死区和滤波参数

## 🔧 配置说明

### WebSocket服务器配置
- **默认端口**: 8765
- **主机地址**: localhost
- **协议**: ws:// (HTTP) 或 wss:// (HTTPS)

### HTTP服务器配置
- **默认端口**: 8080
- **备用端口**: 8081 (如果8080被占用)
- **静态文件**: 自动服务当前目录

## 🐛 故障排除

### 常见问题

#### 1. 端口被占用
```bash
# 查看端口占用
lsof -i :8080
lsof -i :8765

# 杀死占用进程
kill -9 <PID>
```

#### 2. WebSocket连接失败
- 检查WebSocket服务器是否正在运行
- 检查防火墙设置
- 查看浏览器控制台错误信息

#### 3. 依赖安装失败
```bash
# 安装websockets库
pip3 install websockets

# 或使用虚拟环境
python3 -m venv venv
source venv/bin/activate
pip install websockets
```

### 调试模式

#### 启用详细日志
```bash
# 设置环境变量
export DEBUG=1
python3 websocket_server.py
```

#### 浏览器调试
1. 打开浏览器开发者工具 (F12)
2. 查看Console标签页的错误信息
3. 查看Network标签页的WebSocket连接状态

## 🔄 停止服务器

### 使用启动脚本启动的
按 `Ctrl+C` 停止所有服务器

### 手动启动的
```bash
# 查找进程
ps aux | grep -E "(python3|http.server|websocket)"

# 杀死进程
kill -9 <PID>
```

## 🔌 WebSocket API

### 连接
```javascript
const websocket = new WebSocket('ws://localhost:8765');
```

### 消息格式

#### 客户端发送
```json
{
  "type": "request_refresh",  // 请求刷新数据
  "type": "request_logs",     // 请求日志数据
  "type": "ping"              // 心跳检测
}
```

#### 服务器发送
```json
{
  "type": "initial_data",     // 初始数据
  "type": "data_update",      // 数据更新
  "type": "logs_update",      // 日志更新
  "type": "pong"              // 心跳响应
}
```

## 🔄 与C++程序集成

### 当前状态
Web仪表板目前使用模拟数据进行演示。要与实际的C++程序集成，需要：

1. **修改数据源**: 将模拟数据替换为实际数据读取
2. **添加数据接口**: 从C++程序读取数据
3. **实现数据转换**: 将C++数据转换为WebSocket消息格式
4. **配置更新频率**: 根据实际需求调整更新频率

### 集成示例
```python
# 在websocket_server.py中添加实际数据读取
def read_actual_joystick_data():
    # 从C++程序读取实际手柄数据
    # 例如：通过文件、管道、共享内存等
    pass

def read_actual_modbus_data():
    # 从C++程序读取实际Modbus数据
    pass
```

## 📈 性能优化

### 服务器端
- **数据缓存**: 避免重复计算
- **批量更新**: 合并多个数据更新
- **连接池**: 管理WebSocket连接

### 客户端
- **数据节流**: 限制更新频率
- **DOM优化**: 减少DOM操作
- **内存管理**: 及时清理不需要的数据

## 🔒 安全考虑

### 网络安全
- **本地访问**: 默认只允许本地访问
- **端口限制**: 使用非标准端口
- **防火墙**: 配置防火墙规则

### 数据安全
- **数据验证**: 验证输入数据格式
- **错误处理**: 优雅处理异常情况
- **日志记录**: 记录重要操作

## 📝 更新日志

### v1.0.0 (2025-01-08)
- ✨ 初始版本发布
- 🎮 支持手柄数据实时监控
- 📡 支持Modbus数据监控
- 🤖 支持ROS2话题监控
- 📊 现代化Web界面
- 🔄 WebSocket实时通信
- 📋 系统日志监控
- ⚙️ 配置信息显示

## 🤝 贡献指南

欢迎提交Issue和Pull Request来改进这个Web仪表板！

### 开发环境设置
```bash
# 进入web_dashboard目录
cd web_dashboard

# 安装依赖
pip3 install websockets

# 启动开发服务器
./start_web_dashboard.sh
```

### 代码规范
- 使用Python PEP 8规范
- 使用JavaScript ES6+语法
- 添加适当的注释和文档
- 编写单元测试

## 📄 许可证

本项目采用与主项目相同的许可证。

---

*作者: SRS-HUZY*  
*版本: 1.0.0*  
*更新时间: 2025-9-19*
