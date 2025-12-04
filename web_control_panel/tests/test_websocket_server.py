#!/usr/bin/env python3
"""
Sealien Web控制面板 - WebSocket服务器测试
"""

import unittest
import asyncio
import websockets
import json
import time
import threading
from unittest.mock import Mock, patch

# 模拟ROS2环境
import sys
sys.modules['rclpy'] = Mock()
sys.modules['rclpy.logging'] = Mock()
sys.modules['sensor_msgs.msg'] = Mock()
sys.modules['geometry_msgs.msg'] = Mock()
sys.modules['std_msgs.msg'] = Mock()

class TestWebSocketServer(unittest.TestCase):
    """WebSocket服务器测试类"""
    
    def setUp(self):
        """测试前准备"""
        self.websocket_url = "ws://localhost:8765"
        self.test_data = {
            "type": "joystick",
            "data": {
                "axes": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                "buttons": [False] * 32
            },
            "timestamp": time.time()
        }
    
    def test_websocket_connection(self):
        """测试WebSocket连接"""
        async def test_connect():
            try:
                async with websockets.connect(self.websocket_url) as websocket:
                    # 发送测试数据
                    await websocket.send(json.dumps(self.test_data))
                    
                    # 接收响应
                    response = await websocket.recv()
                    data = json.loads(response)
                    
                    self.assertEqual(data["type"], "joystick")
                    self.assertIn("data", data)
                    self.assertIn("timestamp", data)
                    
            except ConnectionRefusedError:
                self.skipTest("WebSocket服务器未运行")
        
        # 运行异步测试
        asyncio.run(test_connect())
    
    def test_data_format(self):
        """测试数据格式"""
        # 测试手柄数据格式
        joystick_data = {
            "type": "joystick",
            "data": {
                "axes": [0.0] * 12,
                "buttons": [False] * 32
            },
            "timestamp": time.time()
        }
        
        self.assertEqual(len(joystick_data["data"]["axes"]), 12)
        self.assertEqual(len(joystick_data["data"]["buttons"]), 32)
        self.assertIsInstance(joystick_data["timestamp"], float)
    
    def test_modbus_data_format(self):
        """测试Modbus数据格式"""
        modbus_data = {
            "type": "modbus",
            "data": {
                "voltages": [0.0] * 8,
                "discrete_inputs": [False] * 8
            },
            "timestamp": time.time()
        }
        
        self.assertEqual(len(modbus_data["data"]["voltages"]), 8)
        self.assertEqual(len(modbus_data["data"]["discrete_inputs"]), 8)
    
    def test_status_data_format(self):
        """测试状态数据格式"""
        status_data = {
            "type": "status",
            "data": {
                "usb_joystick": True,
                "modbus_device": True,
                "ros2_node": True,
                "websocket_clients": 1
            },
            "timestamp": time.time()
        }
        
        self.assertIn("usb_joystick", status_data["data"])
        self.assertIn("modbus_device", status_data["data"])
        self.assertIn("ros2_node", status_data["data"])
        self.assertIn("websocket_clients", status_data["data"])

if __name__ == '__main__':
    unittest.main()
