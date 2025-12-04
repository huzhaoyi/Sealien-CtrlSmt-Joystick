#!/bin/bash
# deploy_to_orin.sh - 一键部署到 Orin NX（使用 SCP，增强诊断功能）

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# 默认配置
ORIN_IP="${1:-192.168.1.100}"      # 第一个参数：Orin NX IP地址
ORIN_USER="${2:-ubuntu}"            # 第二个参数：SSH用户名
DEPLOY_DIR="${3:-/tmp}"              # 第三个参数：远程部署目录
PACKAGE_NAME="sealien-joystick-1.0.0-aarch64.tar.gz"

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
    echo "Sealien Joystick - 一键部署到 Orin NX"
    echo ""
    echo "用法: $0 [IP地址] [用户名] [远程目录]"
    echo ""
    echo "参数:"
    echo "  IP地址      Orin NX 的 IP 地址 (默认: 192.168.1.100)"
    echo "  用户名      SSH 登录用户名 (默认: ubuntu)"
    echo "  远程目录    远程部署目录 (默认: /tmp)"
    echo ""
    echo "示例:"
    echo "  $0                                    # 使用默认配置"
    echo "  $0 192.168.1.100                      # 指定IP"
    echo "  $0 192.168.1.100 ubuntu               # 指定IP和用户"
    echo "  $0 192.168.1.100 ubuntu /home/user    # 指定所有参数"
    echo ""
    echo "诊断选项:"
    echo "  $0 --scan                              # 扫描局域网查找 Orin NX"
    echo "  $0 --test <IP> <用户>                  # 仅测试连接，不传输"
    echo ""
}

# 扫描局域网查找 Orin NX（简单版本）
scan_network() {
    print_info "扫描局域网查找 Orin NX..."
    print_warning "这可能需要一些时间..."
    echo ""
    
    # 获取本机 IP 和网络段
    LOCAL_IP=$(hostname -I | awk '{print $1}')
    NETWORK=$(echo "$LOCAL_IP" | cut -d'.' -f1-3)
    
    print_info "扫描网络段: $NETWORK.0/24"
    print_info "查找开放 SSH 端口的设备..."
    echo ""
    
    FOUND_DEVICES=0
    for i in {1..254}; do
        TEST_IP="$NETWORK.$i"
        if [ "$TEST_IP" = "$LOCAL_IP" ]; then
            continue
        fi
        
        # 检查 SSH 端口
        if command -v nc >/dev/null 2>&1; then
            if nc -z -w 1 "$TEST_IP" 22 2>/dev/null; then
                print_success "发现设备: $TEST_IP (SSH 端口开放)"
                FOUND_DEVICES=$((FOUND_DEVICES + 1))
            fi
        elif command -v timeout >/dev/null 2>&1; then
            if timeout 1 bash -c "echo >/dev/tcp/$TEST_IP/22" 2>/dev/null; then
                print_success "发现设备: $TEST_IP (SSH 端口开放)"
                FOUND_DEVICES=$((FOUND_DEVICES + 1))
            fi
        fi
    done
    
    echo ""
    if [ $FOUND_DEVICES -eq 0 ]; then
        print_warning "未发现开放 SSH 端口的设备"
    else
        print_success "发现 $FOUND_DEVICES 个设备"
        print_info "请尝试使用这些 IP 地址运行部署脚本"
    fi
}

# 仅测试连接
test_connection() {
    TEST_IP="${2:-192.168.1.100}"
    TEST_USER="${3:-ubuntu}"
    
    print_info "测试连接到 $TEST_USER@$TEST_IP..."
    echo ""
    
    # 网络测试
    if ping -c 1 -W 2 "$TEST_IP" >/dev/null 2>&1; then
        print_success "✓ 网络连通"
    else
        print_error "✗ 网络不通"
    fi
    
    # 端口测试
    if command -v nc >/dev/null 2>&1; then
        if nc -z -w 2 "$TEST_IP" 22 2>/dev/null; then
            print_success "✓ SSH 端口开放"
        else
            print_error "✗ SSH 端口未开放"
        fi
    fi
    
    # SSH 连接测试
    if ssh -o ConnectTimeout=5 -o BatchMode=yes -o StrictHostKeyChecking=no "$TEST_USER@$TEST_IP" exit 2>/dev/null; then
        print_success "✓ SSH 免密登录成功"
    else
        print_warning "✗ SSH 免密登录失败，尝试密码登录..."
        if ssh -o ConnectTimeout=10 -o StrictHostKeyChecking=no "$TEST_USER@$TEST_IP" "echo '连接成功'" 2>&1; then
            print_success "✓ SSH 连接成功（需要密码）"
        else
            print_error "✗ SSH 连接失败"
        fi
    fi
}

# 检查参数
if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    show_help
    exit 0
elif [ "$1" = "--scan" ]; then
    scan_network
    exit 0
elif [ "$1" = "--test" ]; then
    test_connection "$@"
    exit 0
fi

print_info "🚀 开始一键部署到 Orin NX..."
echo ""
print_info "配置信息:"
echo "  Orin NX IP: $ORIN_IP"
echo "  SSH 用户: $ORIN_USER"
echo "  远程目录: $DEPLOY_DIR"
echo ""

# 1. 检查部署包
print_info "步骤 1/4: 检查部署包..."
if [ ! -f "$PACKAGE_NAME" ]; then
    print_error "部署包不存在: $PACKAGE_NAME"
    print_info "请先创建部署包:"
    echo "  1. 编译项目: make build-arm64"
    echo "  2. 创建部署包: ./create_release_package.sh"
    echo ""
    print_info "或者使用源码包在目标平台上编译:"
    echo "  ./package_source.sh  # 创建源码包"
    echo "  # 然后传输源码包到目标平台编译"
    exit 1
fi

print_success "部署包已存在: $PACKAGE_NAME"

# 验证部署包架构（仅提示，不阻止传输）
print_diagnostic "验证部署包架构..."
TEMP_DIR=$(mktemp -d)
tar -xzf "$PACKAGE_NAME" -C "$TEMP_DIR" --strip-components=1 sealien-joystick-1.0.0-aarch64/bin/sealien-ctrlsmt-joystick-1.0.0 2>/dev/null || true

if [ -f "$TEMP_DIR/bin/sealien-ctrlsmt-joystick-1.0.0" ]; then
    BINARY_ARCH=$(file "$TEMP_DIR/bin/sealien-ctrlsmt-joystick-1.0.0" | grep -oE "x86-64|aarch64|ARM" || echo "unknown")
    rm -rf "$TEMP_DIR"
    
    if [[ "$BINARY_ARCH" == "x86-64" ]]; then
        print_warning "⚠️  警告: 部署包是 x86-64 架构，无法在 Orin NX (ARM64) 上运行"
        print_info "请重新创建 ARM64 版本的部署包:"
        echo "  make build-arm64"
        echo "  ./create_release_package.sh"
        echo ""
        read -p "是否继续传输？(y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_info "已取消传输"
            exit 0
        fi
    elif [[ "$BINARY_ARCH" == "aarch64" ]] || [[ "$BINARY_ARCH" == "ARM" ]]; then
        print_success "✓ 部署包架构正确 (ARM64)"
    else
        print_warning "⚠️  无法确定二进制文件架构"
    fi
else
    rm -rf "$TEMP_DIR"
    print_warning "⚠️  无法验证部署包架构"
fi

# 检查部署包大小
PACKAGE_SIZE=$(du -h "$PACKAGE_NAME" | cut -f1)
print_info "部署包大小: $PACKAGE_SIZE"

# SSH 连接复用配置（避免多次输入密码）
SSH_CONTROL_DIR="$HOME/.ssh/control"
mkdir -p "$SSH_CONTROL_DIR"
SSH_CONTROL_MASTER="$SSH_CONTROL_DIR/%r@%h:%p"
SSH_OPTS="-o ControlMaster=auto -o ControlPath=$SSH_CONTROL_MASTER -o ControlPersist=60 -o StrictHostKeyChecking=no"

# 2. 网络诊断和 SSH 连接测试
print_info "步骤 2/5: 网络诊断和 SSH 连接测试..."

# 2.1 检查网络连通性
print_diagnostic "检查网络连通性..."
if ping -c 1 -W 2 "$ORIN_IP" >/dev/null 2>&1; then
    print_success "网络连通: $ORIN_IP 可达"
else
    print_error "网络不通: 无法 ping 通 $ORIN_IP"
    print_diagnostic "可能的原因:"
    echo "  - Orin NX 未开机或未联网"
    echo "  - IP 地址错误: $ORIN_IP"
    echo "  - 不在同一网络段"
    echo "  - 防火墙阻止 ICMP"
    echo ""
    print_info "提示: 即使 ping 不通，SSH 仍可能可用（防火墙可能阻止 ping）"
    echo ""
fi

# 2.2 检查 SSH 端口
print_diagnostic "检查 SSH 端口 (22)..."
if command -v nc >/dev/null 2>&1; then
    if nc -z -w 2 "$ORIN_IP" 22 2>/dev/null; then
        print_success "SSH 端口 (22) 开放"
    else
        print_warning "SSH 端口 (22) 可能未开放或被阻止"
    fi
elif command -v timeout >/dev/null 2>&1; then
    if timeout 2 bash -c "echo >/dev/tcp/$ORIN_IP/22" 2>/dev/null; then
        print_success "SSH 端口 (22) 开放"
    else
        print_warning "SSH 端口 (22) 可能未开放或被阻止"
    fi
else
    print_warning "无法检查端口（需要 nc 或 timeout 命令）"
fi

# 2.3 测试 SSH 连接（免密登录）
print_diagnostic "测试 SSH 免密登录..."
if ssh $SSH_OPTS -o ConnectTimeout=5 -o BatchMode=yes "$ORIN_USER@$ORIN_IP" exit 2>/dev/null; then
    print_success "SSH 免密登录成功"
    SSH_NO_PASSWORD=true
else
    print_warning "SSH 免密登录失败，需要密码"
    SSH_NO_PASSWORD=false
fi

# 2.4 测试 SSH 连接（需要密码）
if [ "$SSH_NO_PASSWORD" = false ]; then
    print_diagnostic "尝试 SSH 连接（可能需要输入密码）..."
    print_info "如果提示输入密码，请输入 Orin NX 的登录密码（只需输入一次，后续操作将自动复用连接）"
    echo ""
    
    # 使用 expect 或直接尝试连接
    if ssh $SSH_OPTS -o ConnectTimeout=10 "$ORIN_USER@$ORIN_IP" "echo 'SSH连接测试成功'" 2>&1; then
        print_success "SSH 连接成功"
    else
        SSH_ERROR=$?
        print_error "SSH 连接失败 (错误代码: $SSH_ERROR)"
        echo ""
        print_diagnostic "详细诊断信息:"
        echo ""
        echo "1. 网络检查:"
        if ping -c 1 -W 2 "$ORIN_IP" >/dev/null 2>&1; then
            echo "   ✓ 网络连通"
        else
            echo "   ✗ 网络不通"
        fi
        echo ""
        echo "2. 端口检查:"
        if command -v nc >/dev/null 2>&1 && nc -z -w 2 "$ORIN_IP" 22 2>/dev/null; then
            echo "   ✓ SSH 端口开放"
        else
            echo "   ✗ SSH 端口可能未开放"
        fi
        echo ""
        echo "3. 常见问题排查:"
        echo "   - IP 地址是否正确？当前: $ORIN_IP"
        echo "   - 用户名是否正确？当前: $ORIN_USER"
        echo "   - Orin NX 是否开机？"
        echo "   - SSH 服务是否运行？"
        echo ""
        print_info "在 Orin NX 上运行以下命令检查:"
        echo "  hostname -I          # 查看实际 IP 地址"
        echo "  whoami               # 查看当前用户名"
        echo "  sudo systemctl status ssh  # 检查 SSH 服务状态"
        echo "  sudo systemctl start ssh    # 启动 SSH 服务（如果未运行）"
        echo ""
        print_info "修复建议:"
        echo "  1. 确认 Orin NX 的 IP 地址: ssh 到 Orin NX 运行 'hostname -I'"
        echo "  2. 确认用户名: 在 Orin NX 上运行 'whoami'"
        echo "  3. 启动 SSH 服务: sudo systemctl start ssh"
        echo "  4. 检查防火墙: sudo ufw allow 22/tcp"
        echo ""
        print_info "然后重新运行部署脚本:"
        echo "  ./deploy_to_orin.sh <实际IP> <实际用户名>"
        echo ""
        exit 1
    fi
fi

# 3. 验证远程目录
print_info "步骤 3/5: 验证远程目录..."
print_diagnostic "检查远程目录: $DEPLOY_DIR"
if ssh $SSH_OPTS -o ConnectTimeout=5 "$ORIN_USER@$ORIN_IP" "test -d $DEPLOY_DIR || mkdir -p $DEPLOY_DIR" 2>/dev/null; then
    print_success "远程目录可用: $DEPLOY_DIR"
else
    print_warning "无法创建远程目录，尝试使用 /tmp"
    DEPLOY_DIR="/tmp"
fi

# 4. 传输部署包
print_info "步骤 4/5: 传输部署包到 Orin NX..."
print_info "正在传输 $PACKAGE_NAME 到 $ORIN_USER@$ORIN_IP:$DEPLOY_DIR/"
print_diagnostic "传输包大小: $PACKAGE_SIZE"

# 显示传输进度
if scp $SSH_OPTS "$PACKAGE_NAME" "$ORIN_USER@$ORIN_IP:$DEPLOY_DIR/"; then
    print_success "传输完成！"
    
    # 验证文件是否传输成功（使用连接复用，无需再次输入密码）
    print_diagnostic "验证远程文件..."
    REMOTE_SIZE=$(ssh $SSH_OPTS -o ConnectTimeout=5 "$ORIN_USER@$ORIN_IP" "stat -c%s $DEPLOY_DIR/$PACKAGE_NAME 2>/dev/null || echo 0" 2>/dev/null)
    LOCAL_SIZE=$(stat -c%s "$PACKAGE_NAME" 2>/dev/null || echo 0)
    
    if [ "$REMOTE_SIZE" = "$LOCAL_SIZE" ] && [ "$REMOTE_SIZE" != "0" ]; then
        print_success "文件验证成功 (大小: $REMOTE_SIZE 字节)"
    else
        print_warning "文件大小验证失败，但传输可能已成功"
    fi
else
    print_error "传输失败"
    echo ""
    print_diagnostic "传输失败可能的原因:"
    echo "  - 网络连接中断"
    echo "  - 磁盘空间不足"
    echo "  - 权限问题"
    echo ""
    print_info "请检查网络连接后重试"
    exit 1
fi

# 5. 显示后续操作说明
print_success "步骤 5/5: 传输完成！"
echo ""
echo "=========================================="
print_success "部署包已成功传输到 Orin NX！"
echo "=========================================="
echo ""
print_info "在 Orin NX 上执行以下命令完成安装:"
echo ""
echo "  # 1. 解压部署包"
echo "  cd $DEPLOY_DIR"
echo "  tar -xzf $PACKAGE_NAME"
echo "  cd sealien-joystick-1.0.0-aarch64"
echo ""
echo "  # 2. 安装系统依赖（推荐使用脚本）"
echo "  sudo ./install_dependencies.sh"
echo ""
echo "  或者手动安装:"
echo "  sudo apt update"
echo "  sudo apt install -y libmodbus5 libyaml-cpp0.7 python3-pip"
echo "  pip3 install websockets PyYAML"
echo ""
echo "  # 3. 运行安装脚本"
echo "  sudo ./install.sh"
echo ""
echo "  # 4. 测试运行"
echo "  sealien-joystick --help"
echo ""
print_info "或者使用以下命令一键执行（需要配置 SSH 免密登录）:"
echo "  ssh $ORIN_USER@$ORIN_IP 'cd $DEPLOY_DIR && tar -xzf $PACKAGE_NAME && cd sealien-joystick-1.0.0-aarch64 && sudo ./install.sh'"
echo ""

# 清理 SSH 连接复用（可选，60秒后会自动关闭）
# ssh -O exit "$ORIN_USER@$ORIN_IP" 2>/dev/null || true

