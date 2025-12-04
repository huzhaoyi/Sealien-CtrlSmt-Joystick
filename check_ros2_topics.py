#!/usr/bin/env python3
"""
检查ROS2话题是否有数据
"""
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from geometry_msgs.msg import Twist
from std_msgs.msg import String
import time
import sys

class TopicChecker(Node):
    def __init__(self):
        super().__init__('topic_checker')
        
        self.joystick_received = False
        self.twist_received = False
        self.modbus_raw_received = False
        self.config_received = False
        self.heartbeat_received = False
        
        self.joystick_count = 0
        self.twist_count = 0
        self.modbus_raw_count = 0
        self.config_count = 0
        self.heartbeat_count = 0
        
        # 创建订阅者（使用默认QoS，确保与发布者兼容）
        from rclpy.qos import QoSProfile, ReliabilityPolicy, DurabilityPolicy, HistoryPolicy
        
        # 使用与C++发布者匹配的QoS配置
        # C++使用 create_publisher(topic, 10)，这相当于：
        # - depth=10 (KEEP_LAST)
        # - reliability=RELIABLE
        # - durability=VOLATILE
        default_qos = QoSProfile(
            history=HistoryPolicy.KEEP_LAST,
            depth=10,
            reliability=ReliabilityPolicy.RELIABLE,
            durability=DurabilityPolicy.VOLATILE
        )
        
        self.joystick_sub = self.create_subscription(
            Joy, '/sealien_joystick', self.joystick_callback, default_qos)
        self.twist_sub = self.create_subscription(
            Twist, '/sealien_joystick/cmd_vel', self.twist_callback, default_qos)
        self.modbus_raw_sub = self.create_subscription(
            String, '/sealien_joystick/modbus_raw', self.modbus_raw_callback, default_qos)
        self.config_sub = self.create_subscription(
            String, '/sealien_joystick/config', self.config_callback, default_qos)
        self.heartbeat_sub = self.create_subscription(
            String, '/sealien_joystick/heartbeat', self.heartbeat_callback, default_qos)
        
        self.get_logger().info('已订阅所有话题，等待数据...')
        
        # 等待一段时间让DDS发现发布者，期间进行spin
        import time
        self.get_logger().info('等待DDS发现发布者（通常需要5-10秒）...')
        
        # 检查订阅者是否发现了发布者
        for i in range(100):  # 等待10秒，每次0.1秒
            rclpy.spin_once(self, timeout_sec=0.1)
            
            # 每2秒检查一次订阅者状态
            if i % 20 == 0 and i > 0:
                try:
                    # 尝试获取发布者信息
                    publishers_info = self.get_publishers_info_by_topic('/sealien_joystick')
                    self.get_logger().info(f'  发现 {len(publishers_info)} 个发布者（等待 {i*0.1:.1f} 秒）')
                except:
                    pass
            
            time.sleep(0.1)
        
        self.get_logger().info('DDS发现等待完成，开始接收数据...')
    
    def joystick_callback(self, msg):
        self.joystick_received = True
        self.joystick_count += 1
        if self.joystick_count == 1:
            self.get_logger().info(f'✓ 收到 /sealien_joystick: axes={len(msg.axes)}, buttons={len(msg.buttons)}')
            if len(msg.axes) > 0:
                self.get_logger().info(f'  前5个轴值: {[f"{x:.3f}" for x in msg.axes[:5]]}')
    
    def twist_callback(self, msg):
        self.twist_received = True
        self.twist_count += 1
        if self.twist_count == 1:
            self.get_logger().info(f'✓ 收到 /sealien_joystick/cmd_vel: linear=({msg.linear.x:.3f}, {msg.linear.y:.3f}, {msg.linear.z:.3f}), angular=({msg.angular.x:.3f}, {msg.angular.y:.3f}, {msg.angular.z:.3f})')
    
    def modbus_raw_callback(self, msg):
        self.modbus_raw_received = True
        self.modbus_raw_count += 1
        if self.modbus_raw_count == 1:
            self.get_logger().info(f'✓ 收到 /sealien_joystick/modbus_raw: 数据长度={len(msg.data)}')
    
    def config_callback(self, msg):
        self.config_received = True
        self.config_count += 1
        if self.config_count == 1:
            self.get_logger().info(f'✓ 收到 /sealien_joystick/config: 数据长度={len(msg.data)}')
    
    def heartbeat_callback(self, msg):
        self.heartbeat_received = True
        self.heartbeat_count += 1
        if self.heartbeat_count == 1:
            self.get_logger().info(f'✓ 收到 /sealien_joystick/heartbeat: 数据长度={len(msg.data)}')
    
    def print_summary(self):
        self.get_logger().info('\n=== 话题数据接收摘要 ===')
        self.get_logger().info(f'/sealien_joystick: {"✓" if self.joystick_received else "✗"} (收到 {self.joystick_count} 条消息)')
        self.get_logger().info(f'/sealien_joystick/cmd_vel: {"✓" if self.twist_received else "✗"} (收到 {self.twist_count} 条消息)')
        self.get_logger().info(f'/sealien_joystick/modbus_raw: {"✓" if self.modbus_raw_received else "✗"} (收到 {self.modbus_raw_count} 条消息)')
        self.get_logger().info(f'/sealien_joystick/config: {"✓" if self.config_received else "✗"} (收到 {self.config_count} 条消息)')
        self.get_logger().info(f'/sealien_joystick/heartbeat: {"✓" if self.heartbeat_received else "✗"} (收到 {self.heartbeat_count} 条消息)')
        self.get_logger().info('=======================\n')

def main():
    rclpy.init()
    
    checker = TopicChecker()
    
    # 运行20秒（增加时间以确保有足够时间接收数据）
    timeout = 20.0
    start_time = time.time()
    
    print(f'等待 {timeout} 秒接收数据...\n')
    
    last_check_time = start_time
    check_interval = 2.0  # 每2秒检查一次订阅者状态
    
    try:
        while time.time() - start_time < timeout:
            try:
                rclpy.spin_once(checker, timeout_sec=0.1)
            except Exception as e:
                checker.get_logger().warn(f'Spin错误: {e}')
                break
            
            # 每2秒检查一次订阅者是否发现了发布者
            current_time = time.time()
            if current_time - last_check_time >= check_interval:
                try:
                    # 检查订阅者是否发现了发布者
                    publishers_info = checker.get_publishers_info_by_topic('/sealien_joystick')
                    subscriptions_info = checker.get_subscriptions_info_by_topic('/sealien_joystick')
                    elapsed = current_time - start_time
                    checker.get_logger().info(f'[{elapsed:.1f}s] 发布者: {len(publishers_info)}, 订阅者: {len(subscriptions_info)}, 已收到消息: {checker.joystick_count}')
                    
                    # 如果订阅者已发现但没收到消息，检查QoS
                    if len(subscriptions_info) > 0 and len(publishers_info) > 0 and checker.joystick_count == 0:
                        checker.get_logger().warn(f'  订阅者已发现但未收到消息，可能是QoS不匹配或消息发布过快')
                        # 打印QoS信息
                        for pub_info in publishers_info:
                            checker.get_logger().info(f'    发布者QoS: {pub_info.qos_profile}')
                        for sub_info in subscriptions_info:
                            checker.get_logger().info(f'    订阅者QoS: {sub_info.qos_profile}')
                except Exception as e:
                    checker.get_logger().debug(f'检查发布者/订阅者信息时出错: {e}')
                last_check_time = current_time
            
            # 短暂sleep，避免CPU占用过高
            time.sleep(0.01)
    except KeyboardInterrupt:
        checker.get_logger().info('用户中断')
    except Exception as e:
        checker.get_logger().error(f'运行错误: {e}')
        import traceback
        checker.get_logger().error(traceback.format_exc())
    
    checker.print_summary()
    
    # 检查是否有任何数据
    if not any([checker.joystick_received, checker.twist_received, 
                checker.modbus_raw_received, checker.config_received, 
                checker.heartbeat_received]):
        print('\n⚠️  警告: 没有收到任何话题数据！')
        print('可能的原因:')
        print('  1. C++程序未运行或未启用ROS2')
        print('  2. ROS2域ID不匹配（检查ROS_DOMAIN_ID环境变量）')
        print('  3. 话题名称不匹配')
        print('  4. 发布者未正确初始化')
        sys.exit(1)
    else:
        print('\n✓ 成功接收到话题数据！')
        sys.exit(0)
    
    checker.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

