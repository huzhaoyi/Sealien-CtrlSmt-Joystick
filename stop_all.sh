#!/bin/bash

# Sealien 手柄转换器 - 完全停止脚本
# 终止所有相关的C++程序、WebSocket服务器、HTTP服务器等进程

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 打印消息函数
print_message() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# 检查进程是否存在
check_process() {
    local pattern=$1
    if pgrep -f "$pattern" > /dev/null; then
        return 0
    else
        return 1
    fi
}

# 终止进程
kill_process() {
    local pattern=$1
    local name=$2
    local force=$3
    
    if check_process "$pattern"; then
        local pids=$(pgrep -f "$pattern")
        print_message $YELLOW "正在停止 $name..."
        
        if [ "$force" = "true" ]; then
            # 强制终止
            echo $pids | xargs kill -9 2>/dev/null
            print_message $RED "强制终止 $name (PID: $pids)"
        else
            # 优雅终止
            echo $pids | xargs kill -TERM 2>/dev/null
            sleep 2
            
            # 检查是否还在运行
            if check_process "$pattern"; then
                print_message $YELLOW "$name 仍在运行，强制终止..."
                echo $pids | xargs kill -9 2>/dev/null
                print_message $RED "强制终止 $name (PID: $pids)"
            else
                print_message $GREEN "$name 已停止"
            fi
        fi
    else
        print_message $CYAN "$name 未运行"
    fi
}

# 清理PID文件
cleanup_pid_files() {
    print_message $YELLOW "清理PID文件..."
    
    local pid_files=(
        "web_control_panel/websocket_server.pid"
        "web_control_panel/http_server.pid"
        "web_control_panel/http_port.txt"
    )
    
    for pid_file in "${pid_files[@]}"; do
        if [ -f "$pid_file" ]; then
            rm -f "$pid_file"
            print_message $GREEN "已删除 $pid_file"
        fi
    done
}

# 检查端口占用
check_ports() {
    print_message $YELLOW "检查端口占用情况..."
    
    # Web服务端口
    local web_ports=(8765 8080 8081 8082 8083 8084 8085 8086 8087 8088 8089 8090)
    
    # ROS2常用端口范围
    local ros2_ports=(11311 11312 11313 11314 11315 11316 11317 11318 11319 11320)
    
    # 合并所有端口
    local all_ports=("${web_ports[@]}" "${ros2_ports[@]}")
    
    for port in "${all_ports[@]}"; do
        if lsof -Pi :$port -sTCP:LISTEN -t >/dev/null 2>&1; then
            local pid=$(lsof -Pi :$port -sTCP:LISTEN -t)
            local process=$(ps -p $pid -o comm= 2>/dev/null || echo "unknown")
            print_message $RED "端口 $port 被占用 (PID: $pid, 进程: $process)"
        else
            print_message $GREEN "端口 $port 空闲"
        fi
    done
    
    # 检查是否有其他相关端口被占用
    print_message $YELLOW "检查其他可能的相关端口..."
    local other_ports=$(ss -tuln | grep LISTEN | awk '{print $5}' | cut -d: -f2 | sort -n | uniq | grep -E '^(8[0-9]{3}|9[0-9]{3}|1[0-9]{4}|2[0-9]{4}|3[0-9]{4}|4[0-9]{4}|5[0-9]{4}|6[0-9]{4})' | head -10)
    
    if [ ! -z "$other_ports" ]; then
        print_message $YELLOW "发现其他高端口占用:"
        for port in $other_ports; do
            local pid=$(lsof -Pi :$port -sTCP:LISTEN -t 2>/dev/null)
            if [ ! -z "$pid" ]; then
                local process=$(ps -p $pid -o comm= 2>/dev/null || echo "unknown")
                print_message $CYAN "  端口 $port: PID $pid ($process)"
            fi
        done
    fi
    
    # 检查所有监听的端口（详细模式）
    print_message $YELLOW "检查所有监听的端口..."
    print_message $CYAN "使用 ss 命令:"
    ss -tuln | grep LISTEN | awk '{print $5}' | cut -d: -f2 | sort -n | uniq | while read port; do
        if [ ! -z "$port" ]; then
            pid=$(lsof -Pi :$port -sTCP:LISTEN -t 2>/dev/null)
            if [ ! -z "$pid" ]; then
                process=$(ps -p $pid -o comm= 2>/dev/null || echo "unknown")
                cmdline=$(ps -p $pid -o cmd= 2>/dev/null | cut -c1-80 || echo "unknown")
                print_message $RED "端口 $port: PID $pid ($process)"
                print_message $CYAN "  命令: $cmdline"
            fi
        fi
    done
}

# 清理ROS2话题
cleanup_ros2() {
    print_message $YELLOW "检查ROS2话题..."
    
    if command -v ros2 >/dev/null 2>&1; then
        # 检查sealien相关话题
        local topics=$(ros2 topic list 2>/dev/null | grep sealien || true)
        if [ ! -z "$topics" ]; then
            print_message $YELLOW "发现sealien相关话题:"
            echo "$topics" | while read topic; do
                print_message $CYAN "  - $topic"
            done
        else
            print_message $GREEN "未发现sealien相关话题"
        fi
    else
        print_message $CYAN "ROS2未安装或未在PATH中"
    fi
}

# 主函数
main() {
    print_message $GREEN "🛑 Sealien 手柄转换器 - 完全停止脚本"
    print_message $BLUE "=========================================="
    
    # 停止C++主程序
    kill_process "sealien-ctrlsmt-joystick" "C++主程序" false
    
    # 停止Web控制面板启动脚本
    kill_process "start.sh" "Web控制面板启动脚本" false
    
    # 停止WebSocket服务器
    kill_process "ros2_websocket_server.py" "ROS2 WebSocket服务器" false
    
    # 停止HTTP服务器
    kill_process "http.server" "HTTP服务器" false
    
    # 停止Python HTTP服务器（更具体的模式）
    kill_process "python3 -m http.server" "Python HTTP服务器" false
    
    # 停止任何可能的残留进程
    kill_process "python.*backend.*ros2_websocket_server" "WebSocket服务器残留" true
    kill_process "python.*http.server" "HTTP服务器残留" true
    
    # 等待进程完全停止
    print_message $YELLOW "等待进程完全停止..."
    sleep 3
    
    # 清理PID文件
    cleanup_pid_files
    
    # 检查端口占用
    check_ports
    
    # 清理ROS2话题
    cleanup_ros2
    
    print_message $GREEN "=========================================="
    print_message $GREEN "✅ 所有相关进程已停止"
    print_message $GREEN "=========================================="
    
    # 最终检查
    print_message $YELLOW "最终检查..."
    local remaining_processes=0
    
    if check_process "sealien-ctrlsmt-joystick"; then
        print_message $RED "警告: C++主程序仍在运行"
        remaining_processes=$((remaining_processes + 1))
    fi
    
    if check_process "start.sh"; then
        print_message $RED "警告: Web控制面板启动脚本仍在运行"
        remaining_processes=$((remaining_processes + 1))
    fi
    
    if check_process "ros2_websocket_server"; then
        print_message $RED "警告: WebSocket服务器仍在运行"
        remaining_processes=$((remaining_processes + 1))
    fi
    
    if check_process "http.server"; then
        print_message $RED "警告: HTTP服务器仍在运行"
        remaining_processes=$((remaining_processes + 1))
    fi
    
    if [ $remaining_processes -eq 0 ]; then
        print_message $GREEN "✅ 所有进程已完全停止"
    else
        print_message $RED "⚠️  仍有 $remaining_processes 个进程在运行"
        print_message $YELLOW "请手动检查或使用 'kill -9' 强制终止"
    fi
}

# 如果脚本被直接执行
if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    main "$@"
fi
