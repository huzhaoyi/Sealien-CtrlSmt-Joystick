#!/bin/bash
# Sealien Joystick - 安装免密sudo配置脚本

set -e

echo "=========================================="
echo "Sealien Joystick - 配置免密sudo"
echo "=========================================="
echo ""

# 检查是否以root运行
if [ "$EUID" -ne 0 ]; then 
    echo "此脚本需要root权限，将使用sudo运行"
    echo "请输入密码："
    sudo "$0" "$@"
    exit $?
fi

# 创建sudoers配置
SUDOERS_FILE="/etc/sudoers.d/sealien-joystick"
SUDOERS_CONTENT="# Sealien Joystick - 免密sudo配置
# 允许sealien用户免密运行sealien-ctrlsmt-joystick程序
# 支持绝对路径（run.sh脚本在build目录下使用相对路径，但sudo会解析为绝对路径）
sealien ALL=(ALL) NOPASSWD: /opt/sealien-joystick-1.0.0-source/build/sealien-ctrlsmt-joystick-1.0.0
sealien ALL=(ALL) NOPASSWD: /opt/sealien-joystick-1.0.0-source/build/sealien-ctrlsmt-joystick-1.0.0 *"

echo "创建sudoers配置文件..."
echo "$SUDOERS_CONTENT" > "$SUDOERS_FILE"

echo "设置正确的权限..."
chmod 0440 "$SUDOERS_FILE"

echo "验证sudoers配置..."
if visudo -c -f "$SUDOERS_FILE" 2>/dev/null; then
    echo ""
    echo "✓ sudoers配置已成功安装！"
    echo ""
    echo "配置内容："
    cat "$SUDOERS_FILE"
    echo ""
    echo "现在可以免密运行："
    echo "  sudo /opt/sealien-joystick-1.0.0-source/build/sealien-ctrlsmt-joystick-1.0.0 [配置文件]"
    echo ""
    echo "或者使用run.sh脚本（会自动使用sudo）："
    echo "  ./run.sh config/modbus/AMSAMOTION_MTAE8.yaml"
else
    echo "✗ sudoers配置验证失败，已删除配置文件"
    rm -f "$SUDOERS_FILE"
    exit 1
fi

