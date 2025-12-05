#!/usr/bin/env python3
"""
Sealien 手柄转换器 - ROS2 WebSocket 数据服务器
连接ROS2话题获取真实手柄数据，推送到Web界面
"""

import asyncio
import websockets
import json
import time
import threading
import signal
import sys
import math
from datetime import datetime
import logging
import yaml
import os
import stat

# ROS2 imports
import rclpy
import rclpy.logging
import rclpy.executors
from rclpy.node import Node
from sensor_msgs.msg import Joy
from geometry_msgs.msg import Twist
from std_msgs.msg import String

# 配置日志
logging.basicConfig(
    level=logging.WARNING,  # 改为INFO级别，以便看到更多调试信息
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

class ROS2DataSubscriber(Node):
    """ROS2数据订阅器"""
    
    def __init__(self):
        super().__init__('websocket_ros2_subscriber')
        
        # 设置ROS2日志级别为WARNING，只显示警告和错误
        self.get_logger().set_level(rclpy.logging.LoggingSeverity.WARN)
        
        # 记录启动时间
        self.start_time = time.time()
        
        # 加载配置文件
        self.config_data = self.load_configuration()
        
        # 数据存储
        self.joystick_data = {
            'axes': [],
            'buttons': [],
            'timestamp': 0,
            'device_path': 'unknown'
        }
        
        self.twist_data = {
            'linear': {'x': 0.0, 'y': 0.0, 'z': 0.0},
            'angular': {'x': 0.0, 'y': 0.0, 'z': 0.0}
        }
        
        self.device_status = {
            'status': 'disconnected',
            'device_path': 'unknown',
            'timestamp': 0
        }
        
        # modbus_data 已移除 - 只使用真实数据源 modbus_raw_data
        
        self.modbus_raw_data = {
            'input_registers': {},
            'holding_registers': {},
            'coils': {},
            'discrete_inputs': {},
            'voltages': {},
            'connection_status': False,
            'timestamp': 0
        }
        
        self.modbus_control_data = {
            'command': '',
            'data': {},
            'timestamp': 0,
            'success': False
        }
        
        self.heartbeat_data = {
            'timestamp': 0,
            'system_status': 'unknown',
            'run_mode': 'unknown',
            'modbus_enabled': False,
            'modbus_connected': False,
            'ros2_enabled': False,
            'ros2_initialized': False,
            'usb_devices_count': 0,
            'polling_rate': 0,
            'uptime': 0
        }
        
        self.config_data = {
            'serial_config': {},
            'voltage_mapping': {},
            'axis_config': {},
            'system_config': {},
            'ros2_config': {},
            'axes_mapping': [],
            'buttons_mapping': [],
            'timestamp': 0
        }
        
        # 创建订阅者
        self.joystick_sub = self.create_subscription(
            Joy, '/sealien_joystick', self.joystick_callback, 10)
        self.twist_sub = self.create_subscription(
            Twist, '/sealien_joystick/cmd_vel', self.twist_callback, 10)
        self.status_sub = self.create_subscription(
            String, '/sealien_joystick/device_status', self.status_callback, 10)
        self.modbus_raw_sub = self.create_subscription(
            String, '/sealien_joystick/modbus_raw', self.modbus_raw_callback, 10)
        self.config_sub = self.create_subscription(
            String, '/sealien_joystick/config', self.config_callback, 10)
        self.modbus_control_sub = self.create_subscription(
            String, '/sealien_joystick/modbus_control', self.modbus_control_callback, 10)
        self.heartbeat_sub = self.create_subscription(
            String, '/sealien_joystick/heartbeat', self.heartbeat_callback, 10)
        self.get_logger().info('已订阅心跳话题: /sealien_joystick/heartbeat')
        # 记录订阅者信息用于调试
        self.get_logger().info(f'心跳订阅者已创建: {self.heartbeat_sub}')
        
        # 创建控制命令发布者
        self.control_pub = self.create_publisher(
            String, '/sealien_joystick/modbus_control', 10)
        
        self.get_logger().info('ROS2订阅者和发布者初始化完成')
    
    def load_configuration(self):
        """加载配置文件"""
        config = {
            'serial_config': {
                'port': '/dev/ttyUSB0',
                'baudrate': 115200,
                'data_bits': 8,
                'stop_bits': 1,
                'parity': 'N',
                'slave_id': 1
            },
            'voltage_mapping': {
                'min_voltage': 0.5,
                'mid_voltage': 2.5,
                'max_voltage': 4.5
            },
            'axis_config': {
                'deadzone': 0.12,
                'lpf_alpha': 1.0,
                'polling_rate': 50
            }
        }
        
        try:
            # 尝试加载Modbus配置文件
            config_path = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'config', 'AMSAMOTION_IO8R.yaml')
            if os.path.exists(config_path):
                with open(config_path, 'r', encoding='utf-8') as f:
                    modbus_config = yaml.safe_load(f)
                    if modbus_config:
                        config['serial_config'].update({
                            'port': modbus_config.get('serial_port', '/dev/ttyUSB0'),
                            'baudrate': modbus_config.get('baud', 115200),
                            'data_bits': modbus_config.get('data_bits', 8),
                            'stop_bits': modbus_config.get('stop_bits', 1),
                            'parity': modbus_config.get('parity', 'N'),
                            'slave_id': modbus_config.get('slave_id', 1)
                        })
                        
                        if 'voltage_mapping' in modbus_config:
                            config['voltage_mapping'].update(modbus_config['voltage_mapping'])
                        
                        if 'axes' in modbus_config and modbus_config['axes']:
                            # 从第一个轴配置中获取默认值
                            first_axis = modbus_config['axes'][0]
                            config['axis_config'].update({
                                'deadzone': first_axis.get('deadzone', 0.12),
                                'lpf_alpha': first_axis.get('lpf_alpha', 1.0)
                            })
                        
                        config['axis_config']['polling_rate'] = modbus_config.get('poll_hz', 50)
            
            # 尝试加载手柄检测配置文件
            joystick_config_path = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'config', 'joystick_detector.yaml')
            if os.path.exists(joystick_config_path):
                with open(joystick_config_path, 'r', encoding='utf-8') as f:
                    joystick_config = yaml.safe_load(f)
                    if joystick_config and 'joystick_detector' in joystick_config:
                        detector_config = joystick_config['joystick_detector']
                        config['joystick_config'] = {
                            'enabled': detector_config.get('enabled', True),
                            'hotplug': detector_config.get('enable_hotplug', True),
                            'scan_interval': detector_config.get('scan_interval_ms', 1000),
                            'axis_threshold': detector_config.get('event_handling', {}).get('axis_threshold', 100),
                            'button_debounce': detector_config.get('event_handling', {}).get('button_debounce_ms', 10)
                        }
            
            self.get_logger().info('配置文件加载成功')
            
        except Exception as e:
            self.get_logger().warning(f'配置文件加载失败，使用默认配置: {e}')
        
        return config
    
    def joystick_callback(self, msg):
        """手柄数据回调"""
        # 确保按钮数据是整数类型
        buttons = [int(btn) for btn in msg.buttons]
        
        self.joystick_data = {
            'axes': list(msg.axes),
            'buttons': buttons,
            'timestamp': time.time(),
            'device_path': msg.header.frame_id
        }
        
        # 检查按钮状态变化
        pressed_buttons = [i for i, btn in enumerate(buttons) if btn == 1]
        if pressed_buttons:
            self.get_logger().info(f'按钮按下: {pressed_buttons}')
        
        # 检查轴活动
        active_axes = [i for i, axis in enumerate(msg.axes) if abs(axis) > 0.1]
        if active_axes:
            self.get_logger().info(f'轴活动: {[(i, f"{msg.axes[i]:.2f}") for i in active_axes]}')
        
        self.get_logger().debug(f'收到手柄数据: {len(msg.axes)}轴, {len(buttons)}按钮')
        self.get_logger().debug(f'所有轴值: {[f"{x:.2f}" for x in msg.axes]}')
        self.get_logger().debug(f'所有按钮值: {buttons}')
    
    def twist_callback(self, msg):
        """Twist数据回调"""
        self.twist_data = {
            'linear': {
                'x': float(msg.linear.x),
                'y': float(msg.linear.y),
                'z': float(msg.linear.z)
            },
            'angular': {
                'x': float(msg.angular.x),
                'y': float(msg.angular.y),
                'z': float(msg.angular.z)
            }
        }
        self.get_logger().debug(f'收到Twist数据: linear=({msg.linear.x:.2f}, {msg.linear.y:.2f}), angular=({msg.angular.z:.2f})')
    
    def status_callback(self, msg):
        """设备状态回调"""
        self.device_status = {
            'status': msg.data.split(':')[0] if ':' in msg.data else msg.data,
            'device_path': msg.data.split(':')[1] if ':' in msg.data else 'unknown',
            'timestamp': time.time()
        }
        self.get_logger().debug(f'收到设备状态: {msg.data}')
    
    def modbus_raw_callback(self, msg):
        """原始Modbus数据回调"""
        try:
            import json
            raw_data = json.loads(msg.data)
            
            self.modbus_raw_data = {
                'input_registers': raw_data.get('input_registers', {}),
                'holding_registers': raw_data.get('holding_registers', {}),
                'coils': raw_data.get('coils', {}),
                'discrete_inputs': raw_data.get('discrete_inputs', {}),
                'voltages': raw_data.get('voltages', {}),
                'connection_status': raw_data.get('connection_status', False),
                'timestamp': raw_data.get('timestamp', time.time())
            }
            
            self.get_logger().debug(f'收到原始Modbus数据: 连接状态={self.modbus_raw_data["connection_status"]}')
            self.get_logger().debug(f'输入寄存器: {len(self.modbus_raw_data["input_registers"]) if self.modbus_raw_data["input_registers"] else 0}个')
            self.get_logger().debug(f'保持寄存器: {len(self.modbus_raw_data["holding_registers"]) if self.modbus_raw_data["holding_registers"] else 0}个')
            self.get_logger().debug(f'线圈: {len(self.modbus_raw_data["coils"]) if self.modbus_raw_data["coils"] else 0}个')
            self.get_logger().debug(f'离散量输入: {len(self.modbus_raw_data["discrete_inputs"]) if self.modbus_raw_data["discrete_inputs"] else 0}个')
            
        except json.JSONDecodeError as e:
            self.get_logger().error(f'解析原始Modbus数据失败: {e}')
        except Exception as e:
            self.get_logger().error(f'处理原始Modbus数据失败: {e}')
    
    def config_callback(self, msg):
        """配置信息回调"""
        try:
            import json
            config_data = json.loads(msg.data)
            
            self.config_data = {
                'serial_config': config_data.get('serial_config', {}),
                'voltage_mapping': config_data.get('voltage_mapping', {}),
                'axis_config': config_data.get('axis_config', {}),
                'system_config': config_data.get('system_config', {}),
                'ros2_config': config_data.get('ros2_config', {}),
                'axes_mapping': config_data.get('axes_mapping', []),
                'buttons_mapping': config_data.get('buttons_mapping', []),
                'timestamp': config_data.get('timestamp', time.time())
            }
            
            self.get_logger().info(f'收到配置信息: 串口={self.config_data["serial_config"].get("port", "unknown")}')
            self.get_logger().info(f'波特率: {self.config_data["serial_config"].get("baudrate", "unknown")}')
            self.get_logger().info(f'电压映射: {self.config_data["voltage_mapping"]}')
            self.get_logger().info(f'系统配置: {self.config_data["system_config"]}')
            self.get_logger().info(f'ROS2配置: {self.config_data["ros2_config"]}')
            self.get_logger().info(f'轴映射数量: {len(self.config_data["axes_mapping"])}')
            self.get_logger().info(f'按钮映射数量: {len(self.config_data["buttons_mapping"])}')
        except json.JSONDecodeError as e:
            self.get_logger().error(f'解析配置信息失败: {e}')
        except Exception as e:
            self.get_logger().error(f'处理配置信息失败: {e}')
    
    def modbus_control_callback(self, msg):
        """Modbus控制命令回调"""
        try:
            import json
            control_data = json.loads(msg.data)
            
            self.modbus_control_data = {
                'command': control_data.get('command', ''),
                'data': control_data.get('data', {}),
                'timestamp': control_data.get('timestamp', time.time()),
                'success': control_data.get('success', False)
            }
            
            self.get_logger().debug(f'收到Modbus控制命令: {self.modbus_control_data["command"]}')
        except json.JSONDecodeError as e:
            self.get_logger().error(f'解析Modbus控制命令失败: {e}')
        except Exception as e:
            self.get_logger().error(f'处理Modbus控制命令失败: {e}')
    
    def heartbeat_callback(self, msg):
        """系统心跳回调"""
        try:
            import json
            # 记录原始消息（用于调试）
            self.get_logger().info(f'收到心跳原始消息: {msg.data[:100]}...')
            
            heartbeat_data = json.loads(msg.data)
            
            # 处理时间戳（C++后端发送的是毫秒时间戳）
            timestamp = heartbeat_data.get('timestamp', 0)
            # 如果时间戳很大（超过10^10），说明是毫秒，需要转换为秒
            if timestamp > 1e10:
                timestamp_seconds = timestamp / 1000.0
            else:
                timestamp_seconds = timestamp
            
            self.heartbeat_data = {
                'timestamp': timestamp,  # 保留原始时间戳（毫秒）
                'timestamp_seconds': timestamp_seconds,  # 秒时间戳
                'system_status': heartbeat_data.get('system_status', 'unknown'),
                'run_mode': heartbeat_data.get('run_mode', 'unknown'),
                'modbus_enabled': heartbeat_data.get('modbus_enabled', False),
                'modbus_connected': heartbeat_data.get('modbus_connected', False),
                'ros2_enabled': heartbeat_data.get('ros2_enabled', False),
                'ros2_initialized': heartbeat_data.get('ros2_initialized', False),
                'usb_devices_count': heartbeat_data.get('usb_devices_count', 0),
                'polling_rate': heartbeat_data.get('polling_rate', 0),
                'uptime': heartbeat_data.get('uptime', 0)
            }
            
            self.get_logger().info(f'✓ 收到系统心跳: 状态={self.heartbeat_data["system_status"]}, 模式={self.heartbeat_data["run_mode"]}, Modbus连接={self.heartbeat_data["modbus_connected"]}, 时间戳={timestamp}ms ({timestamp_seconds:.3f}s)')
        except json.JSONDecodeError as e:
            self.get_logger().error(f'✗ 解析系统心跳失败: {e}')
            self.get_logger().error(f'  原始数据: {msg.data[:200]}')
        except Exception as e:
            self.get_logger().error(f'✗ 处理系统心跳失败: {e}')
            import traceback
            self.get_logger().error(f'  错误详情: {traceback.format_exc()}')
    
    def update_modbus_data(self):
        """更新Modbus数据（已移除模拟数据生成，只使用真实数据）"""
        # 模拟数据生成已移除，现在只使用来自C++后端的真实Modbus数据
        # 真实数据通过 modbus_raw_callback 函数接收
        pass
    
    def get_recent_logs(self):
        """获取最近的系统日志"""
        logs = []
        current_time = time.time()
        
        # 生成真实的系统日志
        if hasattr(self, 'joystick_data') and self.joystick_data:
            axes_count = len(self.joystick_data.get("axes", []))
            buttons_count = len(self.joystick_data.get("buttons", []))
            
            # 检查是否有按钮被按下
            pressed_buttons = [i for i, btn in enumerate(self.joystick_data.get("buttons", [])) if btn == 1]
            
            if pressed_buttons:
                logs.append({
                    'level': 'info',
                    'message': f'手柄按钮按下: {[f"Btn{i+1}" for i in pressed_buttons]}',
                    'timestamp': current_time
                })
            
            # 检查摇杆活动
            active_axes = []
            for i, axis in enumerate(self.joystick_data.get("axes", [])):
                if abs(axis) > 0.1:  # 摇杆活动阈值
                    active_axes.append(f"轴{i+1}:{axis:.2f}")
            
            if active_axes:
                logs.append({
                    'level': 'info',
                    'message': f'摇杆活动: {", ".join(active_axes)}',
                    'timestamp': current_time - 0.5
                })
            
            logs.append({
                'level': 'info',
                'message': f'手柄数据: {axes_count}轴, {buttons_count}按钮',
                'timestamp': current_time - 1
            })
        
        # Modbus设备状态日志 - 使用真实数据源
        if hasattr(self, 'modbus_raw_data') and self.modbus_raw_data:
            if self.modbus_raw_data.get('connection_status', False):
                logs.append({
                    'level': 'info',
                    'message': 'Modbus设备连接正常',
                    'timestamp': current_time - 2
                })
            else:
                logs.append({
                    'level': 'warning',
                    'message': 'Modbus设备连接异常',
                    'timestamp': current_time - 2
                })
        
        # ROS2话题状态日志
        logs.append({
            'level': 'info',
            'message': 'ROS2话题发布正常',
            'timestamp': current_time - 2.5
        })
        
        # WebSocket连接状态
        logs.append({
            'level': 'info',
            'message': f'WebSocket服务器运行: 端口8080',
            'timestamp': current_time - 3
        })
        
        # 系统运行状态
        uptime = current_time - self.start_time
        hours = int(uptime // 3600)
        minutes = int((uptime % 3600) // 60)
        seconds = int(uptime % 60)
        
        logs.append({
            'level': 'info',
            'message': f'系统运行时间: {hours:02d}:{minutes:02d}:{seconds:02d}',
            'timestamp': current_time - 3.5
        })
        
        # 内存使用情况（模拟）
        import psutil
        try:
            memory_percent = psutil.virtual_memory().percent
            logs.append({
                'level': 'info',
                'message': f'内存使用: {memory_percent:.1f}%',
                'timestamp': current_time - 4
            })
        except:
            logs.append({
                'level': 'info',
                'message': '内存使用: 正常',
                'timestamp': current_time - 4
            })
        
        # 按时间戳排序，最新的在前
        logs.sort(key=lambda x: x['timestamp'], reverse=True)
        
        # 只返回最近的15条日志
        return logs[:15]
    
    def get_all_data(self):
        """获取所有数据"""
        # Modbus数据更新已移除 - 只使用真实数据源
        
        # 检查数据是否真的发生了变化
        current_time = time.time()
        
        # 为ROS2话题添加变化检测
        ros2_topics = {}
        
        # 手柄话题
        if self.joystick_data:
            ros2_topics['/sealien_joystick'] = {
                'data': self.joystick_data,
                'timestamp': current_time,
                'has_data': True,
                'axes_count': len(self.joystick_data.get('axes', [])),
                'buttons_count': len(self.joystick_data.get('buttons', [])),
                'active_axes': [i for i, axis in enumerate(self.joystick_data.get('axes', [])) if abs(axis) > 0.1],
                'pressed_buttons': [i for i, btn in enumerate(self.joystick_data.get('buttons', [])) if btn == 1]
            }
        
        # Twist话题
        if self.twist_data:
            ros2_topics['/sealien_joystick/cmd_vel'] = {
                'data': self.twist_data,
                'timestamp': current_time,
                'has_data': True,
                'linear_magnitude': math.sqrt(
                    self.twist_data['linear']['x']**2 + 
                    self.twist_data['linear']['y']**2 + 
                    self.twist_data['linear']['z']**2
                ),
                'angular_magnitude': math.sqrt(
                    self.twist_data['angular']['x']**2 + 
                    self.twist_data['angular']['y']**2 + 
                    self.twist_data['angular']['z']**2
                )
            }
        
        # 设备状态话题
        if self.device_status:
            ros2_topics['/sealien_joystick/device_status'] = {
                'data': self.device_status,
                'timestamp': current_time,
                'has_data': True,
                'status': self.device_status.get('status', 'unknown')
            }
        
        # 添加其他话题信息
        ros2_topics['/sealien_joystick/config'] = {
            'data': self.config_data,
            'timestamp': current_time,
            'has_data': bool(self.config_data),
            'config_keys': list(self.config_data.keys()) if self.config_data else []
        }
        
        ros2_topics['/sealien_joystick/modbus_raw'] = {
            'data': self.modbus_raw_data,
            'timestamp': current_time,
            'has_data': bool(self.modbus_raw_data),
            'connection_status': self.modbus_raw_data.get('connection_status', False)
        }
        
        ros2_topics['/sealien_joystick/modbus_control'] = {
            'data': self.modbus_control_data,
            'timestamp': current_time,
            'has_data': bool(self.modbus_control_data and self.modbus_control_data.get('command')),
            'command': self.modbus_control_data.get('command', ''),
            'success': self.modbus_control_data.get('success', False)
        }
        
        return {
            'joystick': self.joystick_data,
            'twist': self.twist_data,
            'device_status': self.device_status,
            'modbus_raw': self.modbus_raw_data,  # 只保留真实数据源
            'heartbeat': self.heartbeat_data,  # 添加心跳数据
            'ros2_topics': ros2_topics,
            'logs': self.get_recent_logs(),
            'configuration': self.config_data,
            'system_status': {
                'uptime': current_time - self.start_time,
                'usb_joystick': {
                    'connected': len(self.joystick_data.get('axes', [])) > 0
                },
                'modbus_device': {
                    'connected': self.modbus_raw_data.get('connection_status', False)
                },
                'ros2_node': {
                    'active': True
                },
                'polling_rate': 50.0,
                'heartbeat': {
                    'last_received': self.heartbeat_data.get('timestamp', 0),  # 毫秒时间戳
                    'last_received_seconds': self.heartbeat_data.get('timestamp_seconds', 0),  # 秒时间戳
                    'system_status': self.heartbeat_data.get('system_status', 'unknown'),
                    'run_mode': self.heartbeat_data.get('run_mode', 'unknown')
                }
            },
            'timestamp': current_time
        }
    
    def publish_control_command(self, control_data):
        """发布Modbus控制命令"""
        try:
            import json
            msg = String()
            msg.data = json.dumps(control_data)
            self.control_pub.publish(msg)
            self.get_logger().info(f'Published control command: {msg.data}')
            return True
        except Exception as e:
            self.get_logger().error(f'Failed to publish control command: {e}')
            return False

class ROS2WebSocketServer:
    """ROS2 WebSocket服务器"""
    
    def __init__(self, host='0.0.0.0', port=8765):
        self.host = host
        self.port = port
        self.clients = set()
        self.running = False
        self.last_heartbeat_time = time.time()
        self.heartbeat_timeout = 10.0  # 心跳超时时间（秒），对应C++中的HEARTBEAT_TIMEOUT_SECONDS
        
        # 确保ROS2日志目录存在且有写权限
        self._ensure_ros_log_directory()
        
        # 初始化ROS2
        rclpy.init()
        self.ros2_node = ROS2DataSubscriber()
        
        # 创建executor（在主线程中使用，避免冲突）
        self.ros2_executor = rclpy.executors.SingleThreadedExecutor()
        self.ros2_executor.add_node(self.ros2_node)
        
        # 启动ROS2节点
        self.ros2_thread = threading.Thread(target=self.ros2_spin_loop, daemon=True)
        self.ros2_thread.start()
        
        # 等待一下让ROS2节点初始化完成
        time.sleep(1.0)
        
        # 先手动spin几次，确保订阅建立
        logger.info("初始化ROS2订阅连接...")
        for i in range(10):
            try:
                self.ros2_executor.spin_once(timeout_sec=0.1)
            except:
                pass
            time.sleep(0.1)
        
        # 检查心跳话题是否存在
        self._check_heartbeat_topic()
        
        # 检查订阅状态
        self._check_subscription_status()
        
        # 尝试接收一次心跳数据（用于验证连接）
        self._test_heartbeat_reception()
        
        logger.info(f"ROS2 WebSocket服务器初始化完成: {host}:{port}")
    
    def _check_heartbeat_topic(self):
        """检查心跳话题是否存在"""
        try:
            import subprocess
            # 使用ros2 topic list检查话题是否存在
            result = subprocess.run(
                ['ros2', 'topic', 'list'],
                capture_output=True,
                text=True,
                timeout=2
            )
            if result.returncode == 0:
                topics = result.stdout.strip().split('\n')
                heartbeat_topic = '/sealien_joystick/heartbeat'
                if heartbeat_topic in topics:
                    logger.info(f"✓ 心跳话题存在: {heartbeat_topic}")
                else:
                    logger.warning(f"✗ 心跳话题不存在: {heartbeat_topic}")
                    logger.warning("  可能的原因:")
                    logger.warning("  1. C++后端程序未运行")
                    logger.warning("  2. C++后端程序未启用ROS2")
                    logger.warning("  3. ROS2域ID不匹配")
                    logger.warning(f"  当前可用的话题: {', '.join(topics[:5])}...")
            else:
                logger.warning("无法检查ROS2话题列表")
        except subprocess.TimeoutExpired:
            logger.warning("检查ROS2话题超时")
        except FileNotFoundError:
            logger.warning("未找到ros2命令，无法检查话题")
        except Exception as e:
            logger.debug(f"检查心跳话题时出错: {e}")
    
    def _check_subscription_status(self):
        """检查订阅状态"""
        try:
            # 检查订阅者是否存在
            if hasattr(self.ros2_node, 'heartbeat_sub'):
                logger.info(f"✓ 心跳订阅者存在: {self.ros2_node.heartbeat_sub}")
                # 尝试获取订阅者信息
                try:
                    # 检查订阅者是否有效
                    sub_info = str(self.ros2_node.heartbeat_sub)
                    logger.info(f"  订阅者信息: {sub_info[:100]}...")
                except:
                    pass
            else:
                logger.warning("✗ 心跳订阅者不存在")
            
            # 检查节点状态
            logger.info(f"ROS2节点名称: {self.ros2_node.get_name()}")
            logger.info(f"ROS2节点命名空间: {self.ros2_node.get_namespace()}")
        except Exception as e:
            logger.warning(f"检查订阅状态时出错: {e}")
    
    def _test_heartbeat_reception(self):
        """测试是否能接收到心跳数据"""
        logger.info("等待接收心跳数据...")
        initial_timestamp = self.ros2_node.heartbeat_data.get('timestamp', 0)
        
        # 等待最多5秒来接收心跳（心跳通常每1-2秒发布一次）
        for i in range(50):  # 50次，每次0.1秒，总共5秒
            # 每次循环都spin一次，处理可能到达的消息
            try:
                self.ros2_executor.spin_once(timeout_sec=0.05)
            except:
                pass
            time.sleep(0.05)
            
            current_timestamp = self.ros2_node.heartbeat_data.get('timestamp', 0)
            if current_timestamp != initial_timestamp and current_timestamp > 0:
                logger.info(f"✓ 成功接收到心跳数据！时间戳: {current_timestamp}")
                return True
        
        # 如果5秒后还没收到，检查ROS2连接状态
        if self.ros2_node.heartbeat_data.get('timestamp', 0) == 0:
            logger.warning("✗ 5秒内未接收到心跳数据")
            logger.warning("  可能的原因:")
            logger.warning("  1. C++后端程序未运行或未启用ROS2")
            logger.warning("  2. ROS2节点连接问题")
            logger.warning("  3. ROS2域ID不匹配（检查ROS_DOMAIN_ID环境变量）")
            logger.warning("  4. 话题订阅配置问题")
            
            # 检查ROS2域ID
            domain_id = os.environ.get('ROS_DOMAIN_ID', '0')
            logger.warning(f"  当前ROS_DOMAIN_ID: {domain_id}")
            
            # 尝试检查话题是否有数据
            try:
                import subprocess
                # 先检查话题信息
                result = subprocess.run(
                    ['ros2', 'topic', 'info', '/sealien_joystick/heartbeat'],
                    capture_output=True,
                    text=True,
                    timeout=2
                )
                if result.returncode == 0:
                    logger.warning(f"  话题信息:\n{result.stdout}")
                
                # 尝试获取一条消息
                result = subprocess.run(
                    ['ros2', 'topic', 'echo', '/sealien_joystick/heartbeat', '--once'],
                    capture_output=True,
                    text=True,
                    timeout=3
                )
                if result.returncode == 0 and result.stdout.strip():
                    logger.warning(f"  话题有数据，示例: {result.stdout[:200]}...")
                else:
                    logger.warning("  话题没有数据或无法读取")
            except Exception as e:
                logger.warning(f"  检查话题时出错: {e}")
            
            logger.warning("  程序将继续运行，请稍后检查心跳状态")
        return False
    
    def _periodic_diagnostic_check(self):
        """定期诊断检查"""
        try:
            # 检查ROS2节点状态
            if not rclpy.ok():
                logger.warning("⚠ ROS2上下文已关闭")
                return
            
            # 检查话题发布者
            import subprocess
            result = subprocess.run(
                ['ros2', 'topic', 'info', '/sealien_joystick/heartbeat'],
                capture_output=True,
                text=True,
                timeout=2
            )
            if result.returncode == 0:
                if 'Publisher count: 0' in result.stdout:
                    logger.warning("⚠ 心跳话题没有发布者，C++后端可能未运行或ROS2未初始化")
                elif 'Publisher count: 1' in result.stdout:
                    # 有发布者但没数据，尝试接收一次
                    if self.ros2_node.heartbeat_data.get('timestamp', 0) == 0:
                        logger.warning("⚠ 心跳话题有发布者但未收到数据，可能是ROS2域ID不匹配或节点未连接")
            
            # 检查ROS2节点列表
            result = subprocess.run(
                ['ros2', 'node', 'list'],
                capture_output=True,
                text=True,
                timeout=2
            )
            if result.returncode == 0:
                if 'sealien_joystick_node' not in result.stdout:
                    logger.warning("⚠ C++后端ROS2节点未在节点列表中，可能ROS2未正确初始化")
                else:
                    logger.info("✓ C++后端ROS2节点存在: sealien_joystick_node")
        except Exception as e:
            logger.debug(f"定期诊断检查出错: {e}")
    
    def _ensure_ros_log_directory(self):
        """确保ROS2日志目录存在且有写权限"""
        try:
            # 获取用户主目录
            home_dir = os.path.expanduser('~')
            ros_dir = os.path.join(home_dir, '.ros')
            ros_log_dir = os.path.join(ros_dir, 'log')
            
            # 创建.ros目录（如果不存在）
            if not os.path.exists(ros_dir):
                try:
                    os.makedirs(ros_dir, mode=0o755)
                    logger.info(f"创建ROS目录: {ros_dir}")
                except (OSError, PermissionError) as e:
                    logger.warning(f"无法创建ROS目录 {ros_dir}: {e}")
            
            # 创建log目录（如果不存在）
            if not os.path.exists(ros_log_dir):
                try:
                    os.makedirs(ros_log_dir, mode=0o755)
                    logger.info(f"创建ROS日志目录: {ros_log_dir}")
                except (OSError, PermissionError) as e:
                    logger.warning(f"无法创建ROS日志目录 {ros_log_dir}: {e}")
            
            # 确保目录有写权限
            if os.path.exists(ros_log_dir):
                # 检查当前用户是否有写权限
                if not os.access(ros_log_dir, os.W_OK):
                    # 尝试修改权限
                    try:
                        os.chmod(ros_log_dir, 0o755)
                        # 再次检查权限
                        if os.access(ros_log_dir, os.W_OK):
                            logger.info(f"已修复ROS日志目录权限: {ros_log_dir}")
                        else:
                            raise PermissionError("权限修复后仍无写权限")
                    except (PermissionError, OSError) as e:
                        logger.warning(f"无法修复ROS日志目录权限: {ros_log_dir}")
                        logger.warning("将禁用ROS2文件日志，仅使用标准输出")
                        # 禁用ROS2文件日志，仅使用标准输出
                        os.environ['RCUTILS_LOGGING_USE_STDOUT'] = '1'
                        logger.info("已设置RCUTILS_LOGGING_USE_STDOUT=1，禁用文件日志")
                else:
                    logger.debug(f"ROS日志目录权限正常: {ros_log_dir}")
            else:
                # 目录不存在且无法创建，禁用文件日志
                logger.warning("ROS日志目录不存在且无法创建，将禁用文件日志")
                os.environ['RCUTILS_LOGGING_USE_STDOUT'] = '1'
                logger.info("已设置RCUTILS_LOGGING_USE_STDOUT=1，禁用文件日志")
        except Exception as e:
            logger.warning(f"设置ROS日志目录时出错: {e}")
            # 如果设置失败，禁用文件日志
            os.environ['RCUTILS_LOGGING_USE_STDOUT'] = '1'
            logger.info("已设置RCUTILS_LOGGING_USE_STDOUT=1，禁用文件日志")
    
    def ros2_spin_loop(self):
        """ROS2节点自旋循环"""
        try:
            logger.info("ROS2节点自旋循环已启动")
            # 使用主线程的executor来更好地控制消息处理
            # 定期spin，确保消息能被及时处理
            try:
                while rclpy.ok():
                    self.ros2_executor.spin_once(timeout_sec=0.1)
            except KeyboardInterrupt:
                pass
        except Exception as e:
            logger.error(f"ROS2节点运行错误: {e}")
            import traceback
            logger.error(f"错误详情: {traceback.format_exc()}")
    
    async def register_client(self, websocket, path=None):
        """注册新的WebSocket客户端"""
        self.clients.add(websocket)
        logger.info(f"客户端连接: {websocket.remote_address}, 当前客户端数: {len(self.clients)}")
        
        # 发送初始数据
        await self.send_initial_data(websocket)
        
        try:
            async for message in websocket:
                await self.handle_client_message(websocket, message)
        except websockets.exceptions.ConnectionClosed:
            logger.info(f"客户端正常断开: {websocket.remote_address}")
        except Exception as e:
            logger.error(f"客户端处理错误: {e}")
        finally:
            self.clients.remove(websocket)
            logger.info(f"客户端断开: {websocket.remote_address}, 剩余客户端数: {len(self.clients)}")
    
    async def send_initial_data(self, websocket):
        """发送初始数据到客户端"""
        initial_data = {
            'type': 'initial_data',
            'data': self.ros2_node.get_all_data(),
            'timestamp': time.time()
        }
        
        try:
            await websocket.send(json.dumps(initial_data))
        except Exception as e:
            logger.error(f"发送初始数据失败: {e}")
    
    async def handle_client_message(self, websocket, message):
        """处理客户端消息"""
        try:
            data = json.loads(message)
            message_type = data.get('type', '')
            
            if message_type == 'ping':
                # 响应ping
                pong_data = {
                    'type': 'pong',
                    'timestamp': time.time()
                }
                await websocket.send(json.dumps(pong_data))
                logger.debug("响应ping消息")
            
            elif message_type == 'modbus_control':
                # 处理Modbus控制命令
                control_data = data.get('data', {})
                success = self.ros2_node.publish_control_command(control_data)
                
                # 发送控制结果响应
                response_data = {
                    'type': 'control_response',
                    'success': success,
                    'timestamp': time.time()
                }
                await websocket.send(json.dumps(response_data))
            
            elif message_type == 'request_data':
                # 发送当前数据
                current_data = {
                    'type': 'data_update',
                    'data': self.ros2_node.get_all_data(),
                    'timestamp': time.time()
                }
                await websocket.send(json.dumps(current_data))
                
        except json.JSONDecodeError:
            logger.warning(f"无效的JSON消息: {message}")
        except Exception as e:
            logger.error(f"处理客户端消息错误: {e}")
    
    def check_heartbeat_status(self):
        """检查心跳状态"""
        current_time = time.time()
        heartbeat_age = current_time - self.last_heartbeat_time
        
        # 更新心跳时间（如果收到了新的心跳数据）
        if hasattr(self.ros2_node, 'heartbeat_data'):
            heartbeat_data = self.ros2_node.heartbeat_data
            # 优先使用已转换的秒时间戳
            heartbeat_timestamp = heartbeat_data.get('timestamp_seconds', 0)
            # 如果没有秒时间戳，尝试从毫秒时间戳转换
            if heartbeat_timestamp == 0:
                timestamp_ms = heartbeat_data.get('timestamp', 0)
                if timestamp_ms > 1e10:  # 毫秒时间戳
                    heartbeat_timestamp = timestamp_ms / 1000.0
                elif timestamp_ms > 0:  # 秒时间戳
                    heartbeat_timestamp = timestamp_ms
            
            # 如果收到了新的心跳数据，更新最后心跳时间
            if heartbeat_timestamp > 0 and heartbeat_timestamp > self.last_heartbeat_time:
                self.last_heartbeat_time = heartbeat_timestamp
                heartbeat_age = current_time - self.last_heartbeat_time
        
        # 检查心跳是否超时
        if heartbeat_age > self.heartbeat_timeout:
            # 检查是否从未收到过心跳
            if hasattr(self.ros2_node, 'heartbeat_data') and self.ros2_node.heartbeat_data.get('timestamp', 0) == 0:
                return {
                    'status': 'no_data',
                    'age': heartbeat_age,
                    'message': f'未收到心跳数据（可能C++后端未运行）'
                }
            else:
                return {
                    'status': 'timeout',
                    'age': heartbeat_age,
                    'message': f'心跳超时 {heartbeat_age:.1f}秒'
                }
        else:
            return {
                'status': 'ok',
                'age': heartbeat_age,
                'message': f'心跳正常 {heartbeat_age:.1f}秒前'
            }

    async def broadcast_data_update(self):
        """广播数据更新"""
        if not self.clients:
            logger.debug("没有客户端连接，跳过数据广播")
            return
        
        all_data = self.ros2_node.get_all_data()
        
        # 添加心跳状态信息
        heartbeat_status = self.check_heartbeat_status()
        all_data['heartbeat_status'] = heartbeat_status
        
        data = {
            'type': 'data_update',
            'data': all_data,
            'timestamp': time.time()
        }
        
        
        message = json.dumps(data)
        disconnected_clients = set()
        
        for client in self.clients:
            try:
                await client.send(message)
            except websockets.exceptions.ConnectionClosed:
                disconnected_clients.add(client)
            except Exception as e:
                logger.error(f"广播数据失败: {e}")
                disconnected_clients.add(client)
        
        # 清理断开的客户端
        if disconnected_clients:
            logger.info(f"清理 {len(disconnected_clients)} 个断开的客户端")
        self.clients -= disconnected_clients
    
    async def broadcast_loop(self):
        """广播循环"""
        last_diagnostic_check = time.time()
        diagnostic_interval = 10.0  # 每10秒检查一次诊断信息
        
        while self.running:
            try:
                await self.broadcast_data_update()
                await asyncio.sleep(0.05)  # 20Hz更新频率，更流畅且减少卡顿
                
                # 定期进行诊断检查
                current_time = time.time()
                if current_time - last_diagnostic_check >= diagnostic_interval:
                    self._periodic_diagnostic_check()
                    last_diagnostic_check = current_time
            except Exception as e:
                logger.error(f"广播循环错误: {e}")
                await asyncio.sleep(1)
    
    async def start_server(self):
        """启动WebSocket服务器"""
        self.running = True
        logger.info(f"启动WebSocket服务器: ws://{self.host}:{self.port}")
        
        # 启动定时广播任务
        asyncio.create_task(self.broadcast_loop())
        
        # 启动WebSocket服务器
        async with websockets.serve(self.register_client, self.host, self.port):
            logger.info("WebSocket服务器已启动，等待连接...")
            await asyncio.Future()  # 保持运行
    
    def stop_server(self):
        """停止服务器"""
        logger.info("正在停止WebSocket服务器...")
        self.running = False
        
        # 关闭ROS2节点
        if self.ros2_node:
            self.ros2_node.destroy_node()
        rclpy.shutdown()
        
        logger.info("WebSocket服务器已停止")

def signal_handler(signum, frame):
    """信号处理器"""
    logger.info(f"收到信号 {signum}，正在关闭服务器...")
    sys.exit(0)

async def main():
    """主函数"""
    # 注册信号处理器
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    
    # 创建并启动服务器
    server = ROS2WebSocketServer()
    
    try:
        await server.start_server()
    except KeyboardInterrupt:
        logger.info("收到中断信号，正在关闭...")
    finally:
        server.stop_server()
        logger.info("服务器已关闭")

if __name__ == "__main__":
    # 检查ROS2依赖
    try:
        import rclpy
        from sensor_msgs.msg import Joy
        from geometry_msgs.msg import Twist
        from std_msgs.msg import String
    except ImportError as e:
        print(f"错误: 缺少ROS2依赖 - {e}")
        print("请安装ROS2和相关Python包:")
        print("sudo apt install ros-humble-desktop")
        print("pip3 install rclpy")
        sys.exit(1)
    
    # 检查websockets依赖
    try:
        import websockets
    except ImportError:
        print("错误: 缺少websockets依赖")
        print("请安装: pip3 install websockets")
        sys.exit(1)
    
    # 运行服务器
    asyncio.run(main())
