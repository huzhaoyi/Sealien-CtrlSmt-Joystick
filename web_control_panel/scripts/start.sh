#!/bin/bash

# Sealien 手柄转换器 - Web控制面板启动器
# 集成启动脚本，自动启动Web控制面板

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 全局变量
WEBSOCKET_PID=""
HTTP_PID=""
HTTP_PORT=8080

# 打印消息函数
print_message() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# 检查依赖
check_dependencies() {
    print_message $YELLOW "检查依赖..."
    
    # 检查Python3
    if ! command -v python3 &> /dev/null; then
        print_message $RED "错误: 未找到Python3"
        exit 1
    fi
    
    # 检查websockets库
    if ! python3 -c "import websockets" 2>/dev/null; then
        print_message $RED "错误: 未安装websockets库"
        print_message $YELLOW "请运行: pip3 install websockets"
        exit 1
    fi
    
    # 检查ROS2
    if ! python3 -c "import rclpy" 2>/dev/null; then
        print_message $RED "错误: 未安装ROS2 Python库"
        print_message $YELLOW "请运行: sudo apt install ros-humble-desktop"
        print_message $YELLOW "或者: pip3 install rclpy"
        exit 1
    fi
    
    # 检查ROS2环境
    if [ -z "$ROS_DISTRO" ]; then
        print_message $YELLOW "警告: ROS2环境未设置，尝试设置..."
        source /opt/ros/humble/setup.bash 2>/dev/null || {
            print_message $RED "错误: 无法设置ROS2环境"
            print_message $YELLOW "请运行: source /opt/ros/humble/setup.bash"
            exit 1
        }
    fi
    
    print_message $GREEN "依赖检查完成"
}

# 检查并修复ROS2日志目录权限
check_ros_log_directory() {
    print_message $YELLOW "检查ROS2日志目录权限..."
    
    local home_dir="$HOME"
    local ros_dir="$home_dir/.ros"
    local ros_log_dir="$ros_dir/log"
    
    # 创建.ros目录（如果不存在）
    if [ ! -d "$ros_dir" ]; then
        mkdir -p "$ros_dir" 2>/dev/null && {
            chmod 755 "$ros_dir" 2>/dev/null
            print_message $GREEN "已创建ROS目录: $ros_dir"
        } || {
            print_message $YELLOW "警告: 无法创建ROS目录: $ros_dir"
        }
    fi
    
    # 创建log目录（如果不存在）
    if [ ! -d "$ros_log_dir" ]; then
        mkdir -p "$ros_log_dir" 2>/dev/null && {
            chmod 755 "$ros_log_dir" 2>/dev/null
            print_message $GREEN "已创建ROS日志目录: $ros_log_dir"
        } || {
            print_message $YELLOW "警告: 无法创建ROS日志目录: $ros_log_dir"
        }
    fi
    
    # 检查写权限
    if [ -d "$ros_log_dir" ]; then
        if [ -w "$ros_log_dir" ]; then
            print_message $GREEN "ROS2日志目录权限正常"
        else
            print_message $YELLOW "警告: ROS2日志目录无写权限，尝试修复..."
            
            # 检查目录所有者
            local dir_owner=$(stat -c '%U' "$ros_log_dir" 2>/dev/null || echo "unknown")
            if [ "$dir_owner" != "$USER" ] && [ "$dir_owner" != "unknown" ]; then
                print_message $YELLOW "目录由用户 $dir_owner 创建，尝试修复所有权..."
                # 尝试使用sudo修复所有权（如果用户有sudo权限）
                if command -v sudo >/dev/null 2>&1; then
                    if sudo chown -R "$USER:$USER" "$ros_dir" 2>/dev/null; then
                        print_message $GREEN "已修复ROS2日志目录所有权"
                    else
                        print_message $YELLOW "无法使用sudo修复权限，将尝试其他方法..."
                    fi
                fi
            fi
            
            # 尝试修复权限
            if chmod 755 "$ros_log_dir" 2>/dev/null; then
                print_message $GREEN "已修复ROS2日志目录权限"
            elif [ -w "$ros_log_dir" ]; then
                # 再次检查，可能所有权修复后就有权限了
                print_message $GREEN "ROS2日志目录权限已正常"
            else
                print_message $YELLOW "警告: 无法自动修复ROS2日志目录权限"
                print_message $YELLOW "程序将尝试使用临时目录作为备选方案"
                print_message $YELLOW "如需手动修复，请运行:"
                print_message $CYAN "  sudo chown -R $USER:$USER $ros_dir"
                print_message $CYAN "  或: chmod 755 $ros_log_dir"
                # 不返回错误，让程序继续运行，Python代码会处理
                return 0
            fi
        fi
    else
        print_message $RED "错误: 无法创建ROS2日志目录"
        return 1
    fi
}

# 启动WebSocket服务器
start_websocket_server() {
    print_message $YELLOW "启动ROS2 WebSocket服务器..."
    
    # 检查是否已有WebSocket服务器在运行
    if [ -f ../websocket_server.pid ]; then
        local ws_pid=$(cat ../websocket_server.pid)
        if ps -p $ws_pid > /dev/null; then
            print_message $YELLOW "WebSocket服务器已在运行 (PID: $ws_pid)"
            return 0
        else
            rm -f ../websocket_server.pid
        fi
    fi
    
    # 检查端口8765是否被占用
    if lsof -Pi :8765 -sTCP:LISTEN -t >/dev/null; then
        print_message $RED "端口8765已被占用，正在尝试停止占用进程..."
        local port_pid=$(lsof -Pi :8765 -sTCP:LISTEN -t)
        if [ ! -z "$port_pid" ]; then
            kill $port_pid 2>/dev/null
            sleep 2
            if lsof -Pi :8765 -sTCP:LISTEN -t >/dev/null; then
                print_message $RED "无法释放端口8765，请手动停止占用进程"
                exit 1
            else
                print_message $GREEN "端口8765已释放"
            fi
        fi
    fi
    
    # 后台启动ROS2 WebSocket服务器
    python3 ../backend/ros2_websocket_server.py &
    WEBSOCKET_PID=$!
    
    # 等待服务器启动
    sleep 3
    
    # 检查服务器是否启动成功
    if ps -p $WEBSOCKET_PID > /dev/null; then
        print_message $GREEN "ROS2 WebSocket服务器已启动 (PID: $WEBSOCKET_PID)"
        echo $WEBSOCKET_PID > ../websocket_server.pid
    else
        print_message $RED "ROS2 WebSocket服务器启动失败"
        exit 1
    fi
}

# 查找可用端口
find_available_port() {
    local start_port=$1
    local max_port=$((start_port + 10))
    
    for ((port=start_port; port<=max_port; port++)); do
        if ! lsof -Pi :$port -sTCP:LISTEN -t >/dev/null; then
            echo $port
            return 0
        fi
    done
    
    echo ""
    return 1
}

# 启动HTTP服务器
start_http_server() {
    print_message $BLUE "启动HTTP服务器..."
    
    # 查找可用端口
    HTTP_PORT=$(find_available_port 8080)
    
    if [ -z "$HTTP_PORT" ]; then
        print_message $RED "错误: 无法找到可用端口 (8080-8090)"
        print_message $YELLOW "请手动停止占用端口的进程或修改脚本中的端口范围"
        exit 1
    fi
    
    if [ "$HTTP_PORT" != "8080" ]; then
        print_message $YELLOW "端口8080已被占用，使用端口 $HTTP_PORT"
    fi
    
    # 启动HTTP服务器（在项目根目录）
    cd .. && python3 backend/http_server.py $HTTP_PORT . &
    HTTP_PID=$!
    
    # 等待服务器启动
    sleep 2
    
    # 检查服务器是否启动成功
    if ps -p $HTTP_PID > /dev/null; then
        print_message $GREEN "HTTP服务器已启动 (PID: $HTTP_PID, 端口: $HTTP_PORT)"
        echo $HTTP_PID > ../http_server.pid
        echo $HTTP_PORT > ../http_port.txt
    else
        print_message $RED "HTTP服务器启动失败"
        exit 1
    fi
}

# 显示访问信息
show_access_info() {
    # 获取本机IP地址
    local LOCAL_IP=$(hostname -I | awk '{print $1}')
    if [ -z "$LOCAL_IP" ]; then
        LOCAL_IP=$(ip route get 8.8.8.8 2>/dev/null | awk '{print $7; exit}')
    fi
    if [ -z "$LOCAL_IP" ]; then
        LOCAL_IP="<本机IP>"
    fi
    
    print_message $GREEN "=========================================="
    print_message $GREEN "🎮 Sealien 手柄转换器 Web控制面板已启动 Note by:Joey_Hu"
    print_message $GREEN "=========================================="
    print_message $CYAN "本地访问地址:"
    print_message $CYAN "  http://localhost:$HTTP_PORT/frontend/web_dashboard.html"
    print_message $CYAN "局域网访问地址:"
    print_message $CYAN "  http://$LOCAL_IP:$HTTP_PORT/frontend/web_dashboard.html"
    print_message $CYAN "WebSocket服务器:"
    print_message $CYAN "  ws://localhost:8765 (本地)"
    print_message $CYAN "  ws://$LOCAL_IP:8765 (局域网)"
    print_message $CYAN "数据源: ROS2话题"
    print_message $CYAN "  - /joystick (手柄数据)"
    print_message $CYAN "  - /cmd_vel (速度命令)"
    print_message $CYAN "  - /device_status (设备状态)"
    print_message $GREEN "=========================================="
    print_message $YELLOW "按 Ctrl+C 停止服务器"
    print_message $GREEN "=========================================="
}

# 清理函数
cleanup() {
    print_message $YELLOW "正在停止服务器..."
    
    # 停止WebSocket服务器
    if [ -f ../websocket_server.pid ]; then
        local ws_pid=$(cat ../websocket_server.pid)
        if ps -p $ws_pid > /dev/null; then
            kill $ws_pid 2>/dev/null
            print_message $GREEN "WebSocket服务器已停止"
        fi
        rm -f ../websocket_server.pid
    fi
    
    # 停止HTTP服务器
    if [ -f ../http_server.pid ]; then
        local http_pid=$(cat ../http_server.pid)
        if ps -p $http_pid > /dev/null; then
            kill $http_pid 2>/dev/null
            print_message $GREEN "HTTP服务器已停止"
        fi
        rm -f ../http_server.pid
    fi
    
    # 清理临时文件
    rm -f ../http_port.txt
    
    print_message $GREEN "服务器已完全停止"
    exit 0
}

# 注册信号处理器
trap cleanup SIGINT SIGTERM

# 主函数
main() {
    print_message $GREEN "🎮 Sealien 手柄转换器 - Web控制面板启动器"
    print_message $BLUE "=========================================="
    
    # 检查依赖
    check_dependencies
    
    # 检查并修复ROS2日志目录权限
    check_ros_log_directory || {
        print_message $RED "ROS2日志目录权限检查失败，但将继续尝试启动..."
    }
    
    # 启动服务器
    start_websocket_server
    start_http_server
    
    # 显示访问信息
    show_access_info
    
    # 等待用户中断
    while true; do
        sleep 1
    done
}

# 运行主函数
main "$@"
