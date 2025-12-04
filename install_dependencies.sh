#!/bin/bash
# install_dependencies.sh - 安装 Sealien Joystick 系统依赖

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
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

print_diagnostic() {
    echo -e "${CYAN}[诊断]${NC} $1"
}

# 显示使用说明
show_help() {
    echo "Sealien Joystick - 依赖安装脚本"
    echo ""
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  --ros2           安装 ROS2 Humble (可选)"
    echo "  --python-only    仅安装 Python 依赖（不安装系统包）"
    echo "  --skip-python    跳过 Python 依赖安装"
    echo "  --help           显示此帮助信息"
    echo ""
    echo "示例:"
    echo "  sudo $0                    # 安装所有必需依赖"
    echo "  sudo $0 --ros2            # 安装所有依赖（包括ROS2）"
    echo "  $0 --python-only          # 仅安装 Python 依赖（不需要sudo）"
    echo ""
}

# 检查是否为 root 用户（安装系统包需要）
check_root() {
    if [ "$EUID" -ne 0 ]; then 
        print_error "安装系统依赖需要 root 权限，请使用 sudo 运行"
        print_info "如果仅安装 Python 依赖，可以使用: $0 --python-only"
        exit 1
    fi
}

# 检测系统类型
detect_system() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$ID
        OS_VERSION=$VERSION_ID
        print_info "检测到系统: $OS $OS_VERSION"
    else
        print_warning "无法检测系统类型，假设为 Ubuntu/Debian"
        OS="ubuntu"
    fi
}

# 安装系统依赖
install_system_dependencies() {
    print_info "📦 安装系统依赖包..."
    
    # 更新包列表
    print_diagnostic "更新包列表..."
    apt update
    
    # 安装必需的系统包
    print_diagnostic "安装必需的系统包..."
    apt install -y \
        libmodbus5 \
        libyaml-cpp0.7 \
        python3-pip
    
    print_success "系统依赖安装完成"
}

# 安装 ROS2 (可选)
install_ros2() {
    print_info "🤖 安装 ROS2 Humble..."
    
    # 检查是否已安装 ROS2
    if [ -d "/opt/ros/humble" ]; then
        print_warning "ROS2 Humble 似乎已安装"
        if [ -t 0 ]; then
            # 交互式终端
            read -p "是否重新安装？(y/N): " -n 1 -r
            echo
            if [[ ! $REPLY =~ ^[Yy]$ ]]; then
                print_info "跳过 ROS2 安装"
                return
            fi
        else
            # 非交互式环境，跳过重新安装
            print_info "检测到非交互式环境，跳过 ROS2 重新安装"
            return
        fi
    fi
    
    # 设置语言环境
    export DEBIAN_FRONTEND=noninteractive
    apt install -y locales
    locale-gen en_US en_US.UTF-8
    update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
    export LANG=en_US.UTF-8
    
    # 添加 ROS2 源
    print_diagnostic "添加 ROS2 源..."
    apt install -y software-properties-common
    add-apt-repository universe
    apt update && apt install -y curl gnupg lsb-release
    
    # 添加 ROS2 GPG 密钥
    curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | apt-key add -
    
    # 添加 ROS2 仓库
    sh -c 'echo "deb [arch=$(dpkg --print-architecture)] http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" > /etc/apt/sources.list.d/ros2-latest.list'
    
    # 更新并安装 ROS2
    apt update
    apt install -y ros-humble-desktop
    
    print_success "ROS2 Humble 安装完成"
    print_info "要使用 ROS2，请运行: source /opt/ros/humble/setup.bash"
    print_info "或将其添加到 ~/.bashrc 中"
}

# 安装 Python 依赖
install_python_dependencies() {
    print_info "🐍 安装 Python 依赖包..."
    
    # 检查是否存在 requirements.txt
    SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
    REQUIREMENTS_FILE=""
    
    # 查找 requirements.txt（可能在当前目录或 web_panel 目录）
    if [ -f "${SCRIPT_DIR}/requirements.txt" ]; then
        REQUIREMENTS_FILE="${SCRIPT_DIR}/requirements.txt"
    elif [ -f "${SCRIPT_DIR}/web_panel/requirements.txt" ]; then
        REQUIREMENTS_FILE="${SCRIPT_DIR}/web_panel/requirements.txt"
    elif [ -f "./web_panel/requirements.txt" ]; then
        REQUIREMENTS_FILE="./web_panel/requirements.txt"
    fi
    
    if [ -z "$REQUIREMENTS_FILE" ]; then
        print_warning "未找到 requirements.txt 文件"
        print_info "尝试安装基本 Python 依赖..."
        
        # 安装基本依赖
        if command -v pip3 &> /dev/null; then
            pip3 install --user websockets PyYAML || {
                print_warning "Python 依赖安装失败，请手动安装"
                return 1
            }
            print_success "基本 Python 依赖安装完成"
        else
            print_error "未找到 pip3，请先安装 python3-pip"
            return 1
        fi
    else
        print_diagnostic "找到依赖文件: $REQUIREMENTS_FILE"
        
        if command -v pip3 &> /dev/null; then
            print_diagnostic "安装 Python 依赖包..."
            pip3 install --user -r "$REQUIREMENTS_FILE" || {
                print_warning "Python 依赖安装失败，尝试使用 sudo..."
                pip3 install -r "$REQUIREMENTS_FILE" || {
                    print_error "Python 依赖安装失败"
                    return 1
                }
            }
            print_success "Python 依赖安装完成"
        else
            print_error "未找到 pip3，请先安装 python3-pip"
            return 1
        fi
    fi
}

# 验证安装
verify_installation() {
    print_info "🔍 验证安装..."
    
    # 检查系统库
    print_diagnostic "检查系统库..."
    MISSING_LIBS=()
    
    if ! ldconfig -p | grep -q libmodbus; then
        MISSING_LIBS+=("libmodbus5")
    fi
    
    if ! ldconfig -p | grep -q libyaml-cpp; then
        MISSING_LIBS+=("libyaml-cpp0.7")
    fi
    
    if [ ${#MISSING_LIBS[@]} -eq 0 ]; then
        print_success "系统库检查通过"
    else
        print_warning "以下库可能未正确安装: ${MISSING_LIBS[*]}"
    fi
    
    # 检查 Python 包
    print_diagnostic "检查 Python 包..."
    if command -v pip3 &> /dev/null; then
        if pip3 show websockets &> /dev/null; then
            print_success "websockets 已安装"
        else
            print_warning "websockets 未安装"
        fi
        
        if pip3 show PyYAML &> /dev/null; then
            print_success "PyYAML 已安装"
        else
            print_warning "PyYAML 未安装"
        fi
    fi
    
    # 检查 ROS2（如果安装了）
    if [ -d "/opt/ros/humble" ]; then
        print_success "ROS2 Humble 已安装"
    fi
}

# 主函数
main() {
    INSTALL_ROS2=false
    PYTHON_ONLY=false
    SKIP_PYTHON=false
    
    # 解析参数
    while [[ $# -gt 0 ]]; do
        case $1 in
            --ros2)
                INSTALL_ROS2=true
                shift
                ;;
            --python-only)
                PYTHON_ONLY=true
                shift
                ;;
            --skip-python)
                SKIP_PYTHON=true
                shift
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            *)
                print_error "未知参数: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    print_info "🚀 开始安装 Sealien Joystick 依赖..."
    echo ""
    
    # 检测系统
    detect_system
    
    # 安装系统依赖（除非仅安装 Python）
    if [ "$PYTHON_ONLY" = false ]; then
        check_root
        install_system_dependencies
        echo ""
    fi
    
    # 安装 ROS2（如果请求）
    if [ "$INSTALL_ROS2" = true ]; then
        check_root
        install_ros2
        echo ""
    fi
    
    # 安装 Python 依赖（除非跳过）
    if [ "$SKIP_PYTHON" = false ]; then
        install_python_dependencies
        echo ""
    fi
    
    # 验证安装
    verify_installation
    echo ""
    
    print_success "依赖安装完成！"
    echo ""
    print_info "下一步:"
    echo "  1. 如果安装了 ROS2，请运行: source /opt/ros/humble/setup.bash"
    echo "  2. 运行安装脚本: sudo ./install.sh"
    echo ""
}

# 运行主函数
main "$@"

