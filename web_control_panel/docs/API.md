# API 文档

## WebSocket API

### 连接信息
- **URL**: `ws://localhost:8765`
- **协议**: WebSocket
- **数据格式**: JSON

### 消息类型

#### 1. 手柄数据 (joystick)
```json
{
  "type": "joystick",
  "data": {
    "axes": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
    "buttons": [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false]
  },
  "timestamp": 1640995200.123
}
```

**字段说明**:
- `axes`: 12个轴的数值 (-1.0 到 1.0)
- `buttons`: 32个按钮状态 (true/false)
- `timestamp`: Unix时间戳

#### 2. Modbus数据 (modbus)
```json
{
  "type": "modbus",
  "data": {
    "voltages": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
    "discrete_inputs": [false, false, false, false, false, false, false, false]
  },
  "timestamp": 1640995200.123
}
```

**字段说明**:
- `voltages`: 8个模拟量输入通道电压值
- `discrete_inputs`: 8个数字输入状态

#### 3. 系统状态 (status)
```json
{
  "type": "status",
  "data": {
    "usb_joystick": true,
    "modbus_device": true,
    "ros2_node": true,
    "websocket_clients": 1,
    "uptime": 3600
  },
  "timestamp": 1640995200.123
}
```

**字段说明**:
- `usb_joystick`: USB手柄连接状态
- `modbus_device`: Modbus设备连接状态
- `ros2_node`: ROS2节点运行状态
- `websocket_clients`: 当前WebSocket客户端数量
- `uptime`: 系统运行时间(秒)

#### 4. 日志数据 (logs)
```json
{
  "type": "logs",
  "data": {
    "level": "INFO",
    "message": "WebSocket客户端连接",
    "module": "websocket_server",
    "timestamp": "2025-09-19 10:30:00"
  },
  "timestamp": 1640995200.123
}
```

**字段说明**:
- `level`: 日志级别 (DEBUG, INFO, WARNING, ERROR)
- `message`: 日志消息
- `module`: 模块名称
- `timestamp`: 格式化时间戳

## HTTP API

### 静态文件服务
- **基础URL**: `http://localhost:8080`
- **前端页面**: `http://localhost:8080/frontend/web_dashboard.html`

### 健康检查
- **端点**: `GET /health`
- **响应**: 
```json
{
  "status": "healthy",
  "timestamp": 1640995200.123,
  "services": {
    "websocket": true,
    "ros2": true,
    "modbus": true
  }
}
```

## 错误处理

### WebSocket错误
```json
{
  "type": "error",
  "data": {
    "code": "CONNECTION_FAILED",
    "message": "无法连接到ROS2节点",
    "timestamp": 1640995200.123
  }
}
```

### HTTP错误
- **404**: 页面未找到
- **500**: 服务器内部错误
- **503**: 服务不可用

## 使用示例

### JavaScript客户端
```javascript
const ws = new WebSocket('ws://localhost:8765');

ws.onopen = function() {
    console.log('WebSocket连接已建立');
};

ws.onmessage = function(event) {
    const data = JSON.parse(event.data);
    
    switch(data.type) {
        case 'joystick':
            updateJoystickDisplay(data.data);
            break;
        case 'modbus':
            updateModbusDisplay(data.data);
            break;
        case 'status':
            updateStatusDisplay(data.data);
            break;
        case 'logs':
            addLogEntry(data.data);
            break;
    }
};

ws.onerror = function(error) {
    console.error('WebSocket错误:', error);
};

ws.onclose = function() {
    console.log('WebSocket连接已关闭');
};
```

### Python客户端
```python
import asyncio
import websockets
import json

async def client():
    uri = "ws://localhost:8765"
    async with websockets.connect(uri) as websocket:
        while True:
            message = await websocket.recv()
            data = json.loads(message)
            print(f"收到数据: {data['type']}")

asyncio.run(client())
```
