#!/usr/bin/env python3
"""
调试ROS2发现机制
"""
import rclpy
from rclpy.node import Node
import time
import sys

class DiscoveryChecker(Node):
    def __init__(self):
        super().__init__('discovery_checker')
        
    def check_topics(self):
        """检查话题发现情况"""
        self.get_logger().info('检查ROS2话题发现情况...')
        
        # 获取所有话题
        topic_names_and_types = self.get_topic_names_and_types()
        
        self.get_logger().info(f'发现 {len(topic_names_and_types)} 个话题:')
        for topic_name, topic_types in topic_names_and_types:
            self.get_logger().info(f'  {topic_name}: {topic_types}')
            
            # 检查是否有发布者
            try:
                publishers_info = self.get_publishers_info_by_topic(topic_name)
                self.get_logger().info(f'    发布者数量: {len(publishers_info)}')
                for pub_info in publishers_info:
                    self.get_logger().info(f'      发布者: {pub_info.node_name} (QoS: {pub_info.qos_profile})')
            except Exception as e:
                self.get_logger().warn(f'    无法获取发布者信息: {e}')
            
            # 检查是否有订阅者
            try:
                subscriptions_info = self.get_subscriptions_info_by_topic(topic_name)
                self.get_logger().info(f'    订阅者数量: {len(subscriptions_info)}')
                for sub_info in subscriptions_info:
                    self.get_logger().info(f'      订阅者: {sub_info.node_name} (QoS: {sub_info.qos_profile})')
            except Exception as e:
                self.get_logger().warn(f'    无法获取订阅者信息: {e}')
    
    def check_nodes(self):
        """检查节点发现情况"""
        self.get_logger().info('检查ROS2节点发现情况...')
        
        node_names = self.get_node_names()
        self.get_logger().info(f'发现 {len(node_names)} 个节点:')
        for node_name in node_names:
            self.get_logger().info(f'  {node_name}')

def main():
    rclpy.init()
    
    checker = DiscoveryChecker()
    
    # 等待DDS发现
    print('等待DDS发现（5秒）...')
    for i in range(50):
        rclpy.spin_once(checker, timeout_sec=0.1)
        time.sleep(0.1)
    
    print('\n=== ROS2发现信息 ===\n')
    checker.check_nodes()
    print()
    checker.check_topics()
    
    checker.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()

