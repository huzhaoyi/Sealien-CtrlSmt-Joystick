#!/bin/bash

# Sealien手柄程序启动脚本
# 集成启动、监听、检测等功能

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
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

# 显示使用帮助
show_help() {
    echo "Sealien手柄程序启动脚本"
    echo ""
    echo "用法: $0 [选项] [配置文件]"
    echo ""
    echo "启动选项:"
    echo "  -h, --help              显示此帮助信息"
    echo "  -d, --domain-id ID      指定ROS2域ID (默认: 0)"
    echo "  -a, --auto-detect       自动检测可用的ROS2域ID"
    echo "  -c, --check-env         检查ROS2环境"
    echo "  --no-ros2              禁用ROS2功能"
    echo ""
    echo "ROS2监听选项:"
    echo "  -l, --list-topics       列出所有话题"
    echo "  -j, --joy               监听手柄数据话题"
    echo "  -t, --twist             监听速度命令话题"
    echo "  -s, --status            监听设备状态话题"
    echo "  -i, --info TOPIC        显示话题信息"
    echo "  -f, --frequency TOPIC   显示话题发布频率"
    echo "  --once                  只接收一次消息"
    echo ""
    echo "配置文件:"
    echo "  如果不指定配置文件，程序将运行在USB模式（无需配置文件）"
    echo "  只有需要Modbus功能时才需要指定配置文件"
    echo ""
    echo "示例:"
    echo "  $0                                    # 启动程序 (USB模式，无需配置文件)"
    echo "  $0 config/modbus/AMSAMOTION_IO8R.yaml # 启动程序 (Modbus模式)"
    echo "  $0 config/custom.yaml                # 启动程序 (自定义配置)"
    echo "  $0 -d 1 config/custom.yaml           # 使用域ID 1和自定义配置"
    echo "  $0 -a                                 # 自动检测域ID"
    echo "  $0 -j                                 # 监听手柄数据话题"
    echo "  $0 -l                                 # 列出所有话题"
    echo "  $0 --once -j                          # 只接收一次手柄数据"
    echo ""
}

# 检查ROS2环境
check_ros2_env() {
    print_info "检查ROS2环境..."
    
    if [ ! -f "/opt/ros/humble/setup.bash" ]; then
        print_error "未找到ROS2 Humble环境"
        return 1
    fi
    
    source /opt/ros/humble/setup.bash
    
    if ! command -v ros2 >/dev/null 2>&1; then
        print_error "ROS2命令不可用"
        return 1
    fi
    
    print_success "ROS2环境检查通过"
    return 0
}

# 自动检测可用的ROS2域ID
auto_detect_domain() {
    print_info "自动检测可用的ROS2域ID..."
    
    if ! check_ros2_env; then
        print_warning "ROS2环境不可用，使用默认域ID 0"
        export ROS_DOMAIN_ID=0
        return 0
    fi
    
    local test_domain=0
    local max_domains=10
    
    for ((i=0; i<max_domains; i++)); do
        export ROS_DOMAIN_ID=$test_domain
        
        # 检查此域是否有活跃节点
        local node_count=$(timeout 2 ros2 node list 2>/dev/null | wc -l)
        if [ "$node_count" -eq 0 ]; then
            print_success "找到可用域ID: $test_domain"
            export ROS_DOMAIN_ID=$test_domain
            return 0
        else
            print_info "域ID $test_domain 已被占用 ($node_count 个节点)"
        fi
        
        ((test_domain++))
    done
    
    print_warning "无法找到完全空闲的域ID，使用域ID 0"
    export ROS_DOMAIN_ID=0
    return 0
}

# 列出所有话题
list_topics() {
    print_info "列出所有ROS2话题..."
    
    if ! check_ros2_env; then
        return 1
    fi
    
    local topics=$(ros2 topic list 2>/dev/null)
    if [ -z "$topics" ]; then
        print_warning "没有找到任何话题"
        return 1
    fi
    
    print_success "找到以下话题:"
    echo "$topics" | while read -r topic; do
        if [ -n "$topic" ]; then
            echo "  - $topic"
        fi
    done
}

# 显示话题信息
show_topic_info() {
    local topic="$1"
    
    if [ -z "$topic" ]; then
        print_error "请指定话题名称"
        return 1
    fi
    
    print_info "显示话题信息: $topic"
    
    if ! check_ros2_env; then
        return 1
    fi
    
    ros2 topic info "$topic" 2>/dev/null || {
        print_error "无法获取话题信息: $topic"
        return 1
    }
}

# 显示话题发布频率
show_topic_frequency() {
    local topic="$1"
    
    if [ -z "$topic" ]; then
        print_error "请指定话题名称"
        return 1
    fi
    
    print_info "显示话题发布频率: $topic"
    print_info "按Ctrl+C停止"
    
    if ! check_ros2_env; then
        return 1
    fi
    
    ros2 topic hz "$topic"
}

# 监听话题
listen_topic() {
    local topic="$1"
    local once="$2"
    
    if [ -z "$topic" ]; then
        print_error "请指定话题名称"
        return 1
    fi
    
    print_info "监听话题: $topic"
    if [ "$once" = true ]; then
        print_info "模式: 单次接收"
    else
        print_info "模式: 持续监听 (按Ctrl+C停止)"
    fi
    
    if ! check_ros2_env; then
        return 1
    fi
    
    if [ "$once" = true ]; then
        ros2 topic echo "$topic" --once
    else
        ros2 topic echo "$topic"
    fi
}

# 检查输入设备权限
check_input_permissions() {
    print_info "检查输入设备权限..."
    
    local js_devices=()
    for device in /dev/input/js*; do
        if [ -c "$device" ]; then
            js_devices+=("$device")
        fi
    done
    
    if [ ${#js_devices[@]} -eq 0 ]; then
        print_warning "未找到手柄设备"
        return 0
    fi
    
    # 检查用户是否在input组中
    if ! groups | grep -q input; then
        print_warning "用户不在input组中，可能需要sudo权限"
        return 1
    fi
    
    print_success "输入设备权限检查通过"
    return 0
}

# 检查uinput模块（Modbus模式需要）
check_uinput() {
    print_info "检查uinput模块..."
    
    # 检查模块是否加载
    if ! lsmod | grep -q "^uinput"; then
        print_warning "uinput模块未加载"
        print_info "Modbus模式需要uinput模块来创建虚拟手柄设备"
        print_info "请运行: sudo modprobe uinput"
        return 1
    fi
    
    # 检查设备文件是否存在
    if [ ! -e "/dev/uinput" ]; then
        print_warning "uinput设备文件不存在: /dev/uinput"
        print_info "即使模块已加载，设备文件可能尚未创建"
        print_info "请尝试: sudo modprobe uinput 或重启系统"
        return 1
    fi
    
    # 检查设备文件权限
    if [ ! -r "/dev/uinput" ] || [ ! -w "/dev/uinput" ]; then
        print_warning "uinput设备权限不足"
        print_info "请运行: sudo chmod 666 /dev/uinput"
        print_info "或添加udev规则: echo 'KERNEL==\"uinput\", MODE=\"0666\"' | sudo tee /etc/udev/rules.d/99-uinput.rules"
        return 1
    fi
    
    print_success "uinput模块检查通过"
    return 0
}

# 自动选择默认配置文件
select_default_config() {
    local config_dir="config"
    
    # 检查config目录是否存在
    if [ ! -d "$config_dir" ]; then
        print_error "配置目录不存在: $config_dir"
        return 1
    fi
    
    # 动态搜索config目录中的所有YAML文件
    local yaml_files=()
    while IFS= read -r -d '' file; do
        yaml_files+=("$file")
    done < <(find "$config_dir" -maxdepth 1 -name "*.yaml" -type f -print0 2>/dev/null)
    
    # 检查是否找到YAML文件
    if [ ${#yaml_files[@]} -eq 0 ]; then
        print_error "在 $config_dir 目录中未找到任何YAML配置文件"
        print_info "请确保至少有一个.yaml文件存在于config目录中"
        return 1
    fi
    
    # 按文件名排序，选择第一个
    local sorted_files=($(printf '%s\n' "${yaml_files[@]}" | sort))
    local selected_file="${sorted_files[0]}"
    
    # 输出调试信息到stderr，避免影响返回值
    print_info "在 $config_dir 目录中找到 ${#yaml_files[@]} 个YAML文件:" >&2
    for file in "${sorted_files[@]}"; do
        local basename_file=$(basename "$file")
        if [ "$file" = "$selected_file" ]; then
            echo "  ✓ $basename_file (已选择)" >&2
        else
            echo "  - $basename_file" >&2
        fi
    done
    
    echo "$selected_file"
    return 0
}

# 主函数
main() {
    local config_file=""
    local domain_id=""
    local auto_detect=false
    local no_ros2=false
    local check_env=false
    local list_topics_flag=false
    local joy_topic=false
    local twist_topic=false
    local status_topic=false
    local topic_info=""
    local topic_frequency=""
    local once=false
    
    # 解析命令行参数
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -d|--domain-id)
                domain_id="$2"
                shift 2
                ;;
            -a|--auto-detect)
                auto_detect=true
                shift
                ;;
            -c|--check-env)
                check_env=true
                shift
                ;;
            --no-ros2)
                no_ros2=true
                shift
                ;;
            -l|--list-topics)
                list_topics_flag=true
                shift
                ;;
            -j|--joy)
                joy_topic=true
                shift
                ;;
            -t|--twist)
                twist_topic=true
                shift
                ;;
            -s|--status)
                status_topic=true
                shift
                ;;
            -i|--info)
                topic_info="$2"
                shift 2
                ;;
            -f|--frequency)
                topic_frequency="$2"
                shift 2
                ;;
            --once)
                once=true
                shift
                ;;
            -*)
                print_error "未知选项: $1"
                show_help
                exit 1
                ;;
            *)
                # 如果第一个参数不是选项，则认为是配置文件
                if [ -z "$config_file" ]; then
                    config_file="$1"
                else
                    print_error "只能指定一个配置文件"
                    show_help
                    exit 1
                fi
                shift
                ;;
        esac
    done
    
    # 如果没有指定配置文件，使用USB模式
    if [ -z "$config_file" ]; then
        print_info "未指定配置文件，将运行在USB模式"
    fi
    
    # 设置ROS2域ID
    if [ -n "$domain_id" ]; then
        export ROS_DOMAIN_ID=$domain_id
        print_info "使用指定的ROS2域ID: $domain_id"
    elif [ "$auto_detect" = true ]; then
        auto_detect_domain
    else
        export ROS_DOMAIN_ID=${ROS_DOMAIN_ID:-0}
        print_info "使用ROS2域ID: $ROS_DOMAIN_ID"
    fi
    
    # 设置ROS2环境变量
    export RCUTILS_LOGGING_SEVERITY_THRESHOLD=${RCUTILS_LOGGING_SEVERITY_THRESHOLD:-INFO}
    
    # 确保ROS_DOMAIN_ID被明确设置并显示
    if [ -z "$ROS_DOMAIN_ID" ]; then
        export ROS_DOMAIN_ID=0
    fi
    print_info "当前ROS2域ID: $ROS_DOMAIN_ID (确保ros2 topic echo使用相同的域ID)"
    
    # 处理特殊命令
    if [ "$check_env" = true ]; then
        check_ros2_env
        exit $?
    fi
    
    if [ "$list_topics_flag" = true ]; then
        list_topics
        exit 0
    fi
    
    if [ -n "$topic_info" ]; then
        show_topic_info "$topic_info"
        exit 0
    fi
    
    if [ -n "$topic_frequency" ]; then
        show_topic_frequency "$topic_frequency"
        exit 0
    fi
    
    # 处理ROS2监听命令
    if [ "$joy_topic" = true ] || [ "$twist_topic" = true ] || [ "$status_topic" = true ]; then
        if [ "$no_ros2" = false ]; then
            if ! check_ros2_env; then
                print_warning "ROS2环境不可用，无法监听话题"
                exit 1
            fi
        fi
        
        # 确定要监听的话题
        local topics=()
        if [ "$joy_topic" = true ]; then
            topics+=("/sealien_joystick")
        fi
        if [ "$twist_topic" = true ]; then
            topics+=("/sealien_joystick/cmd_vel")
        fi
        if [ "$status_topic" = true ]; then
            topics+=("/sealien_joystick/device_status")
        fi
        
        # 监听话题
        for topic in "${topics[@]}"; do
            if [ "$once" = true ]; then
                listen_topic "$topic" true
            else
                listen_topic "$topic" false
            fi
        done
        exit 0
    fi
    
    # 检查配置文件（如果指定了的话）
    if [ -n "$config_file" ] && [ ! -f "$config_file" ]; then
        print_error "配置文件不存在: $config_file"
        exit 1
    fi
    
    # 设置ROS2环境
    if [ "$no_ros2" = false ]; then
        if check_ros2_env; then
            print_success "ROS2环境已配置"
        else
            print_warning "ROS2环境配置失败，继续运行但可能影响ROS2功能"
        fi
    else
        print_info "ROS2功能已禁用"
    fi
    
    # 检查输入设备权限
    if ! check_input_permissions; then
        print_warning "输入设备权限检查失败"
        print_info "要避免使用sudo，请运行权限设置脚本:"
        print_info "  sudo ./setup_permissions.sh"
        print_info "然后重新登录或运行: newgrp input"
        print_warning "将使用sudo运行程序（如果权限不足）"
    fi
    
    # 如果指定了配置文件（Modbus模式），检查uinput模块
    if [ -n "$config_file" ]; then
        if ! check_uinput; then
            print_warning "uinput模块检查失败"
            print_info "要避免使用sudo，请运行权限设置脚本:"
            print_info "  sudo ./setup_permissions.sh"
            if [ "$no_ros2" = false ]; then
                print_info "已启用ROS2功能，程序将仅使用ROS2输出模式（不使用虚拟手柄）"
                print_info "注意: 如果系统不支持uinput（如Tegra/Jetson），这是正常的"
            else
                print_error "uinput不可用且未启用ROS2，Modbus模式无法正常工作"
                print_info "修复步骤:"
                print_info "  1. 运行权限设置脚本: sudo ./setup_permissions.sh"
                print_info "  2. 或启用ROS2功能（推荐，适用于Tegra系统）"
                print_info "  3. 或手动运行: sudo modprobe uinput && sudo chmod 666 /dev/uinput"
                exit 1
            fi
        fi
    fi
    
    # 设置库路径
export LD_LIBRARY_PATH=/opt/ros/humble/lib:/opt/ros/humble/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH

# 切换到项目根目录
cd "$(dirname "$0")"

    # 检查可执行文件
    local executable="./build/sealien-ctrlsmt-joystick-1.0.0"
    if [ ! -f "$executable" ]; then
        print_error "可执行文件不存在: $executable"
        print_info "请先编译项目: make"
        exit 1
    fi
    
    
    # 显示启动信息
    print_info "启动Sealien手柄程序..."
    if [ -n "$config_file" ]; then
        print_info "配置文件: $config_file"
    else
        print_info "模式: USB手柄（无需配置文件）"
    fi
    if [ "$no_ros2" = false ]; then
        print_info "ROS2域ID: $ROS_DOMAIN_ID"
    fi
    
    # 切换到build目录
cd build
    
    # 运行程序
    if groups | grep -q input; then
        print_info "以普通用户权限运行..."
        print_info "ROS2域ID: $ROS_DOMAIN_ID (使用此域ID运行ros2命令)"
        if [ -n "$config_file" ]; then
            ROS_DOMAIN_ID=$ROS_DOMAIN_ID LD_LIBRARY_PATH=/opt/ros/humble/lib:/opt/ros/humble/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH ./$(basename "$executable") "../$config_file"
        else
            ROS_DOMAIN_ID=$ROS_DOMAIN_ID LD_LIBRARY_PATH=/opt/ros/humble/lib:/opt/ros/humble/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH ./$(basename "$executable")
        fi
    else
        print_info "以sudo权限运行..."
        print_info "ROS2域ID: $ROS_DOMAIN_ID (使用此域ID运行ros2命令)"
        if [ -n "$config_file" ]; then
            sudo -E ROS_DOMAIN_ID=$ROS_DOMAIN_ID LD_LIBRARY_PATH=/opt/ros/humble/lib:/opt/ros/humble/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH ./$(basename "$executable") "../$config_file"
        else
            sudo -E ROS_DOMAIN_ID=$ROS_DOMAIN_ID LD_LIBRARY_PATH=/opt/ros/humble/lib:/opt/ros/humble/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH ./$(basename "$executable")
        fi
    fi
}

# 运行主函数
main "$@"
