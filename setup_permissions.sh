#!/bin/bash

# Sealien手柄程序权限设置脚本
# 用于设置输入设备和uinput的权限，避免使用sudo运行

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

# 检查是否为root用户
if [ "$EUID" -ne 0 ]; then 
    print_error "此脚本需要root权限"
    print_info "请使用: sudo $0"
    exit 1
fi

print_info "开始设置Sealien手柄程序权限..."

# 1. 将当前用户添加到input组
CURRENT_USER=${SUDO_USER:-$USER}
if [ -z "$CURRENT_USER" ] || [ "$CURRENT_USER" = "root" ]; then
    print_warning "无法确定当前用户，请手动运行: sudo usermod -a -G input YOUR_USERNAME"
else
    print_info "将用户 $CURRENT_USER 添加到input组..."
    if usermod -a -G input "$CURRENT_USER" 2>/dev/null; then
        print_success "用户 $CURRENT_USER 已添加到input组"
        print_warning "请重新登录或运行 'newgrp input' 以使组权限生效"
    else
        print_warning "无法添加用户到input组（可能已经存在）"
    fi
fi

# 2. 加载uinput模块（如果未加载）
print_info "检查uinput模块..."
if ! lsmod | grep -q "^uinput"; then
    print_info "加载uinput模块..."
    if modprobe uinput; then
        print_success "uinput模块已加载"
    else
        print_error "无法加载uinput模块"
        exit 1
    fi
else
    print_success "uinput模块已加载"
fi

# 3. 设置uinput设备权限（临时）
print_info "设置uinput设备权限..."
if [ -e "/dev/uinput" ]; then
    if chmod 666 /dev/uinput; then
        print_success "uinput设备权限已设置（临时）"
    else
        print_warning "无法设置uinput设备权限"
    fi
else
    print_warning "uinput设备文件不存在，可能需要重启系统"
fi

# 4. 创建udev规则（永久设置）
print_info "创建udev规则..."

# uinput规则
UDEV_RULE_FILE="/etc/udev/rules.d/99-sealien-uinput.rules"
if [ ! -f "$UDEV_RULE_FILE" ]; then
    cat > "$UDEV_RULE_FILE" << 'EOF'
# Sealien手柄程序 - uinput设备权限规则
# 允许所有用户访问uinput设备
KERNEL=="uinput", MODE="0666", GROUP="input"
EOF
    print_success "udev规则已创建: $UDEV_RULE_FILE"
else
    print_info "udev规则已存在: $UDEV_RULE_FILE"
fi

# 输入设备规则（如果需要）
INPUT_RULE_FILE="/etc/udev/rules.d/99-sealien-input.rules"
if [ ! -f "$INPUT_RULE_FILE" ]; then
    cat > "$INPUT_RULE_FILE" << 'EOF'
# Sealien手柄程序 - 输入设备权限规则
# 允许input组访问所有输入设备
KERNEL=="js[0-9]*", GROUP="input", MODE="0664"
KERNEL=="event[0-9]*", GROUP="input", MODE="0664"
EOF
    print_success "输入设备udev规则已创建: $INPUT_RULE_FILE"
else
    print_info "输入设备udev规则已存在: $INPUT_RULE_FILE"
fi

# 5. 重新加载udev规则
print_info "重新加载udev规则..."
if udevadm control --reload-rules && udevadm trigger; then
    print_success "udev规则已重新加载"
else
    print_warning "无法重新加载udev规则，可能需要重启系统"
fi

# 6. 设置uinput模块自动加载
print_info "设置uinput模块自动加载..."
MODULES_FILE="/etc/modules-load.d/sealien-uinput.conf"
if [ ! -f "$MODULES_FILE" ]; then
    echo "uinput" > "$MODULES_FILE"
    print_success "uinput模块已设置为自动加载"
else
    print_info "uinput模块自动加载配置已存在"
fi

print_success "权限设置完成！"
echo ""
print_info "下一步操作："
if [ -n "$CURRENT_USER" ] && [ "$CURRENT_USER" != "root" ]; then
    echo "  1. 重新登录或运行: newgrp input"
    echo "  2. 验证权限: groups | grep input"
    echo "  3. 运行程序: ./run.sh config/modbus/AMSAMOTION_MTAE8.yaml"
else
    echo "  1. 将用户添加到input组: sudo usermod -a -G input YOUR_USERNAME"
    echo "  2. 重新登录"
    echo "  3. 运行程序: ./run.sh config/modbus/AMSAMOTION_MTAE8.yaml"
fi

