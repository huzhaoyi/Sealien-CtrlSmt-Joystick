#!/bin/bash

# ROS2话题诊断脚本

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_info "ROS2话题诊断工具"
echo ""

# 检查ROS2环境
if [ ! -f "/opt/ros/humble/setup.bash" ]; then
    print_error "未找到ROS2 Humble环境"
    exit 1
fi

source /opt/ros/humble/setup.bash

# 检查ROS2域ID
print_info "检查ROS2域ID..."
if [ -z "$ROS_DOMAIN_ID" ]; then
    print_warning "ROS_DOMAIN_ID未设置，使用默认值0"
    print_warning "如果程序使用不同的域ID，将无法通信！"
    print_info "请确保ros2命令使用相同的域ID:"
    print_info "  export ROS_DOMAIN_ID=0"
    print_info "  ros2 topic echo /sealien_joystick/heartbeat"
    export ROS_DOMAIN_ID=0
else
    print_success "ROS_DOMAIN_ID=$ROS_DOMAIN_ID"
fi

# 列出所有话题
print_info "列出所有ROS2话题..."
topics=$(ros2 topic list 2>/dev/null)
if [ -z "$topics" ]; then
    print_warning "没有找到任何话题"
    print_info "可能原因："
    print_info "  1. 程序未运行"
    print_info "  2. ROS2域ID不匹配"
    print_info "  3. ROS2发现机制未完成（需要等待10-30秒）"
else
    echo "$topics"
    echo ""
    
    # 检查heartbeat话题
    if echo "$topics" | grep -q "/sealien_joystick/heartbeat"; then
        print_success "找到heartbeat话题: /sealien_joystick/heartbeat"
        echo ""
        
        # 显示话题信息
        print_info "话题详细信息:"
        ros2 topic info /sealien_joystick/heartbeat
        echo ""
        
        # 显示话题类型
        print_info "话题类型:"
        ros2 topic type /sealien_joystick/heartbeat
        echo ""
        
        # 显示话题频率（如果可能）
        print_info "尝试接收一次消息（等待5秒）..."
        timeout 5 ros2 topic echo /sealien_joystick/heartbeat --once 2>&1 || {
            print_warning "5秒内未收到消息"
            print_info "可能原因："
            print_info "  1. 发布者未发布消息"
            print_info "  2. QoS不匹配"
            print_info "  3. ROS2发现机制未完成"
        }
    else
        print_warning "未找到heartbeat话题: /sealien_joystick/heartbeat"
        print_info "可能原因："
        print_info "  1. 程序未运行或heartbeat未发布"
        print_info "  2. ROS2域ID不匹配"
        print_info "  3. 需要等待ROS2发现机制完成"
    fi
fi

echo ""
print_info "检查ROS2节点..."
nodes=$(ros2 node list 2>/dev/null)
if [ -z "$nodes" ]; then
    print_warning "没有找到任何节点"
else
    echo "$nodes"
fi

echo ""
print_info "诊断完成"
print_info "如果heartbeat话题存在但收不到消息，请尝试："
print_info "  1. 确保程序正在运行"
print_info "  2. 检查ROS2域ID是否一致: echo \$ROS_DOMAIN_ID"
print_info "  3. 等待更长时间（ROS2发现需要10-30秒）"
print_info "  4. 先运行: ros2 topic echo /sealien_joystick/heartbeat"
print_info "  5. 然后启动程序"

