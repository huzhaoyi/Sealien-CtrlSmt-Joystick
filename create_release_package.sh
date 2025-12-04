#!/bin/bash
# create_release_package.sh - 创建产品部署包（不含源码）

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 项目信息
PROJECT_NAME="sealien-joystick"
VERSION="1.0.0"
ARCH="aarch64"  # Orin NX 架构
RELEASE_DIR="${PROJECT_NAME}-${VERSION}-${ARCH}"
BUILD_DIR="build"
EXECUTABLE_NAME="sealien-ctrlsmt-joystick-1.0.0"

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

print_info "📦 开始创建产品部署包..."

# 1. 检查是否已编译
print_info "检查编译产物..."
if [ ! -f "${BUILD_DIR}/${EXECUTABLE_NAME}" ]; then
    print_error "可执行文件不存在: ${BUILD_DIR}/${EXECUTABLE_NAME}"
    print_info "请先编译项目:"
    echo "  对于 x86_64 主机: make build"
    echo "  对于 ARM64 (Orin NX): make build-arm64"
    exit 1
fi

# 检查二进制文件架构
BINARY_ARCH=$(file "${BUILD_DIR}/${EXECUTABLE_NAME}" | grep -oE "x86-64|aarch64|ARM" || echo "unknown")
if [[ "$BINARY_ARCH" == "x86-64" ]]; then
    print_warning "检测到二进制文件是 x86-64 架构，但部署目标是 ARM64 (Orin NX)"
    print_info "请使用交叉编译: make build-arm64"
    print_info "或者直接在 Orin NX 上编译: make build"
    exit 1
elif [[ "$BINARY_ARCH" == "aarch64" ]] || [[ "$BINARY_ARCH" == "ARM" ]]; then
    print_success "找到可执行文件: ${BUILD_DIR}/${EXECUTABLE_NAME} (ARM64)"
else
    print_warning "无法确定二进制文件架构，继续打包..."
    print_success "找到可执行文件: ${BUILD_DIR}/${EXECUTABLE_NAME}"
fi

# 2. 检查配置文件目录
if [ ! -d "config" ]; then
    print_error "配置文件目录不存在: config"
    exit 1
fi

# 3. 检查网页控制面板
WEB_PANEL_DIR="web_control_panel"
HAS_WEB_PANEL=false
if [ -d "${WEB_PANEL_DIR}" ]; then
    if [ -f "${WEB_PANEL_DIR}/backend/ros2_websocket_server.py" ] && [ -d "${WEB_PANEL_DIR}/frontend" ]; then
        HAS_WEB_PANEL=true
        print_success "找到网页控制面板"
    else
        print_warning "网页控制面板目录存在但文件不完整，跳过网页打包"
    fi
else
    print_warning "未找到网页控制面板目录，跳过网页打包"
fi

# 4. 创建发布目录结构
print_info "创建发布目录结构..."
rm -rf "${RELEASE_DIR}"
if [ "$HAS_WEB_PANEL" = true ]; then
    mkdir -p "${RELEASE_DIR}"/{bin,config,scripts,web_panel/{backend,frontend,scripts,config}}
else
    mkdir -p "${RELEASE_DIR}"/{bin,config,scripts}
fi

# 5. 复制可执行文件
print_info "复制可执行文件..."
cp "${BUILD_DIR}/${EXECUTABLE_NAME}" "${RELEASE_DIR}/bin/"
chmod +x "${RELEASE_DIR}/bin/${EXECUTABLE_NAME}"

# 6. 复制配置文件
print_info "复制配置文件..."
cp -r config/* "${RELEASE_DIR}/config/"

# 7. 打包网页控制面板（如果存在）
if [ "$HAS_WEB_PANEL" = true ]; then
    print_info "打包网页控制面板..."
    
    # 复制后端文件（Python 文件，运行需要源码）
    if [ -d "${WEB_PANEL_DIR}/backend" ]; then
        cp -r "${WEB_PANEL_DIR}/backend"/*.py "${RELEASE_DIR}/web_panel/backend/" 2>/dev/null || true
        print_success "已复制后端文件"
    fi
    
    # 复制前端文件（HTML/CSS/JS）
    if [ -d "${WEB_PANEL_DIR}/frontend" ]; then
        cp -r "${WEB_PANEL_DIR}/frontend"/* "${RELEASE_DIR}/web_panel/frontend/" 2>/dev/null || true
        print_success "已复制前端文件"
    fi
    
    # 复制启动脚本（并修复路径以适配打包后的结构）
    if [ -f "${WEB_PANEL_DIR}/scripts/start.sh" ]; then
        # 读取原始脚本并在开头添加路径设置
        cat > "${RELEASE_DIR}/web_panel/scripts/start.sh" << 'WEB_START_EOF'
#!/bin/bash

# Sealien 手柄转换器 - Web控制面板启动器
# 产品版本 - 路径已适配

# 获取脚本所在目录（适配打包后的结构）
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
WEB_PANEL_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

# 设置工作目录
cd "${WEB_PANEL_ROOT}"

WEB_START_EOF
        # 复制原始脚本内容，但跳过第一行的shebang（已经添加了）
        tail -n +2 "${WEB_PANEL_DIR}/scripts/start.sh" >> "${RELEASE_DIR}/web_panel/scripts/start.sh"
        
        # 修复脚本中的路径引用
        sed -i 's|python3 ../backend/ros2_websocket_server.py|python3 "${WEB_PANEL_ROOT}/backend/ros2_websocket_server.py"|g' \
            "${RELEASE_DIR}/web_panel/scripts/start.sh" || true
        sed -i 's|cd .. && python3 -m http.server|cd "${WEB_PANEL_ROOT}" \&\& python3 -m http.server|g' \
            "${RELEASE_DIR}/web_panel/scripts/start.sh" || true
        sed -i 's|../websocket_server.pid|"${WEB_PANEL_ROOT}/websocket_server.pid"|g' \
            "${RELEASE_DIR}/web_panel/scripts/start.sh" || true
        sed -i 's|../http_server.pid|"${WEB_PANEL_ROOT}/http_server.pid"|g' \
            "${RELEASE_DIR}/web_panel/scripts/start.sh" || true
        sed -i 's|../http_port.txt|"${WEB_PANEL_ROOT}/http_port.txt"|g' \
            "${RELEASE_DIR}/web_panel/scripts/start.sh" || true
        sed -i 's|../frontend/web_dashboard.html|"${WEB_PANEL_ROOT}/frontend/web_dashboard.html"|g' \
            "${RELEASE_DIR}/web_panel/scripts/start.sh" || true
        
        chmod +x "${RELEASE_DIR}/web_panel/scripts/start.sh"
        print_success "已复制启动脚本（已适配路径）"
    fi
    
    # 复制配置示例
    if [ -f "${WEB_PANEL_DIR}/config/config.yaml.example" ]; then
        mkdir -p "${RELEASE_DIR}/web_panel/config"
        cp "${WEB_PANEL_DIR}/config/config.yaml.example" "${RELEASE_DIR}/web_panel/config/"
        print_success "已复制配置示例"
    fi
    
    # 复制依赖文件
    if [ -f "${WEB_PANEL_DIR}/requirements.txt" ]; then
        cp "${WEB_PANEL_DIR}/requirements.txt" "${RELEASE_DIR}/web_panel/"
        print_success "已复制依赖文件"
    fi
    
    # 复制资源文件（如果有）
    if [ -d "${WEB_PANEL_DIR}/assets" ]; then
        cp -r "${WEB_PANEL_DIR}/assets" "${RELEASE_DIR}/web_panel/" 2>/dev/null || true
        print_success "已复制资源文件"
    fi
    
    print_success "网页控制面板打包完成"
fi

# 8. 创建适配ARM64的启动脚本
print_info "创建启动脚本（ARM64适配）..."
cat > "${RELEASE_DIR}/scripts/run.sh" << 'SCRIPT_EOF'
#!/bin/bash
# Sealien手柄程序启动脚本 (ARM64版本)
# 适配 Orin NX 架构

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
EXECUTABLE="${PROJECT_ROOT}/bin/sealien-ctrlsmt-joystick-1.0.0"

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 设置ROS2库路径 (ARM64架构)
export LD_LIBRARY_PATH=/opt/ros/humble/lib:/opt/ros/humble/lib/aarch64-linux-gnu:${LD_LIBRARY_PATH}

# 设置ROS2域ID（如果未设置）
export ROS_DOMAIN_ID=${ROS_DOMAIN_ID:-0}

# 切换到项目根目录
cd "${PROJECT_ROOT}"

# 检查可执行文件
if [ ! -f "${EXECUTABLE}" ]; then
    print_error "可执行文件不存在: ${EXECUTABLE}"
    exit 1
fi

# 显示启动信息
print_info "启动 Sealien 手柄程序..."
if [ -n "$1" ]; then
    print_info "配置文件: $1"
    "${EXECUTABLE}" "$1"
else
    print_info "模式: USB手柄（无需配置文件）"
    "${EXECUTABLE}"
fi
SCRIPT_EOF
chmod +x "${RELEASE_DIR}/scripts/run.sh"

# 9. 创建安装脚本
print_info "创建安装脚本..."
cat > "${RELEASE_DIR}/install.sh" << 'INSTALL_EOF'
#!/bin/bash
# Sealien Joystick 产品安装脚本

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

INSTALL_DIR="/opt/sealien-joystick"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

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

print_info "📥 安装 Sealien Joystick 产品..."

# 检查权限
if [ "$EUID" -ne 0 ]; then 
    print_error "请使用 sudo 运行安装脚本"
    exit 1
fi

# 创建安装目录
print_info "创建安装目录: ${INSTALL_DIR}"
mkdir -p "${INSTALL_DIR}"

# 复制文件
print_info "复制文件到 ${INSTALL_DIR}..."
cp -r "${SCRIPT_DIR}/bin" "${INSTALL_DIR}/"
cp -r "${SCRIPT_DIR}/config" "${INSTALL_DIR}/"
cp -r "${SCRIPT_DIR}/scripts" "${INSTALL_DIR}/"

# 复制网页控制面板（如果存在）
if [ -d "${SCRIPT_DIR}/web_panel" ]; then
    print_info "安装网页控制面板..."
    cp -r "${SCRIPT_DIR}/web_panel" "${INSTALL_DIR}/"
    
    # 设置网页控制面板脚本权限
    if [ -f "${INSTALL_DIR}/web_panel/scripts/start.sh" ]; then
        chmod +x "${INSTALL_DIR}/web_panel/scripts/start.sh"
    fi
    
    # 检查并安装Python依赖
    if [ -f "${INSTALL_DIR}/web_panel/requirements.txt" ]; then
        print_info "检查Python依赖..."
        if command -v pip3 &> /dev/null; then
            print_info "安装Python依赖包..."
            pip3 install -r "${INSTALL_DIR}/web_panel/requirements.txt" --user || {
                print_warning "Python依赖安装失败，请手动运行: pip3 install -r ${INSTALL_DIR}/web_panel/requirements.txt"
            }
        else
            print_warning "未找到pip3，请手动安装Python依赖"
        fi
    fi
    
    print_success "网页控制面板安装完成"
fi

# 设置权限
chmod +x "${INSTALL_DIR}/bin/sealien-ctrlsmt-joystick-1.0.0"
chmod +x "${INSTALL_DIR}/scripts/run.sh"

# 创建符号链接（可选，方便命令行使用）
if [ ! -L /usr/local/bin/sealien-joystick ]; then
    print_info "创建命令行快捷方式..."
    ln -sf "${INSTALL_DIR}/scripts/run.sh" /usr/local/bin/sealien-joystick
    print_success "已创建快捷方式: sealien-joystick"
fi

# 创建网页控制面板快捷方式（如果存在）
if [ -d "${INSTALL_DIR}/web_panel" ] && [ ! -L /usr/local/bin/sealien-web-panel ]; then
    print_info "创建网页控制面板快捷方式..."
    ln -sf "${INSTALL_DIR}/web_panel/scripts/start.sh" /usr/local/bin/sealien-web-panel
    print_success "已创建快捷方式: sealien-web-panel"
fi

print_success "安装完成！"
echo ""
echo "使用方法:"
echo "  手柄程序:"
echo "    ${INSTALL_DIR}/scripts/run.sh [配置文件]"
echo "    或"
echo "    sealien-joystick [配置文件]"
if [ -d "${INSTALL_DIR}/web_panel" ]; then
    echo ""
    echo "  网页控制面板:"
    echo "    ${INSTALL_DIR}/web_panel/scripts/start.sh"
    echo "    或"
    echo "    sealien-web-panel"
fi
echo ""
echo "配置文件位置: ${INSTALL_DIR}/config/"
if [ -d "${INSTALL_DIR}/web_panel" ]; then
    echo "网页控制面板位置: ${INSTALL_DIR}/web_panel/"
fi
INSTALL_EOF
chmod +x "${RELEASE_DIR}/install.sh"

# 9.5. 复制依赖安装脚本
print_info "复制依赖安装脚本..."
if [ -f "install_dependencies.sh" ]; then
    cp "install_dependencies.sh" "${RELEASE_DIR}/"
    chmod +x "${RELEASE_DIR}/install_dependencies.sh"
    print_success "已复制依赖安装脚本"
else
    print_warning "未找到 install_dependencies.sh，跳过复制"
fi

# 10. 创建部署说明文档
print_info "创建部署说明文档..."
cat > "${RELEASE_DIR}/README_DEPLOY.md" << 'README_EOF'
# Sealien Joystick 产品部署说明

## 📋 系统要求

- **操作系统**: Ubuntu 20.04+ (ARM64架构)
- **CPU架构**: ARM64 (aarch64) - 适用于 Orin NX
- **依赖库**: 
  - `libmodbus5` - Modbus通信库
  - `libyaml-cpp0.7` - YAML配置文件解析库
  - `ROS2 Humble` (可选) - 如果使用ROS2功能

## 🔧 安装系统依赖

### 方法一：使用依赖安装脚本（推荐）

解压部署包后，运行依赖安装脚本：

```bash
# 安装所有必需依赖
sudo ./install_dependencies.sh

# 如果需要 ROS2 功能
sudo ./install_dependencies.sh --ros2

# 仅安装 Python 依赖（不需要 sudo）
./install_dependencies.sh --python-only
```

### 方法二：手动安装

在 Orin NX 上执行以下命令安装依赖：

```bash
sudo apt update
sudo apt install -y libmodbus5 libyaml-cpp0.7 python3-pip
```

如果使用ROS2功能，还需要安装：
```bash
sudo apt install -y ros-humble-desktop
```

如果使用网页控制面板，还需要安装Python依赖：
```bash
pip3 install websockets PyYAML
# 或使用 requirements.txt
pip3 install -r web_panel/requirements.txt
```

## 📦 安装产品

### 方法一：使用安装脚本（推荐）

```bash
# 解压部署包
tar -xzf sealien-joystick-1.0.0-aarch64.tar.gz
cd sealien-joystick-1.0.0-aarch64

# 安装系统依赖（如果尚未安装）
sudo ./install_dependencies.sh

# 运行安装脚本
sudo ./install.sh
```

### 方法二：手动安装

```bash
# 创建安装目录
sudo mkdir -p /opt/sealien-joystick

# 复制文件
sudo cp -r bin config scripts /opt/sealien-joystick/

# 设置权限
sudo chmod +x /opt/sealien-joystick/bin/sealien-ctrlsmt-joystick-1.0.0
sudo chmod +x /opt/sealien-joystick/scripts/run.sh
```

## 🚀 使用方法

### USB模式（无需配置文件）

```bash
/opt/sealien-joystick/scripts/run.sh
```

或使用快捷方式：
```bash
sealien-joystick
```

### Modbus模式（指定配置文件）

```bash
/opt/sealien-joystick/scripts/run.sh /opt/sealien-joystick/config/modbus/AMSAMOTION_IO8R.yaml
```

或使用快捷方式：
```bash
sealien-joystick /opt/sealien-joystick/config/modbus/AMSAMOTION_IO8R.yaml
```

### 网页控制面板

```bash
/opt/sealien-joystick/web_panel/scripts/start.sh
```

或使用快捷方式：
```bash
sealien-web-panel
```

启动后，在浏览器中访问：
```
http://localhost:8080/frontend/web_dashboard.html
```

注意：网页控制面板需要先启动手柄程序才能获取数据。

## ⚙️ 配置说明

### 手柄程序配置

配置文件位于: `/opt/sealien-joystick/config/`

根据实际设备修改相应的配置文件：
- `config/modbus/` - Modbus设备配置
- `config/joystick/` - 手柄映射配置
- `config/usb/` - USB厂商配置

### 网页控制面板配置

配置文件位于: `/opt/sealien-joystick/web_panel/config/`

复制配置示例并修改：
```bash
cp /opt/sealien-joystick/web_panel/config/config.yaml.example /opt/sealien-joystick/web_panel/config/config.yaml
# 然后编辑 config.yaml 文件
```

## 🔍 故障排除

### 检查依赖库

如果遇到库依赖问题，检查：
```bash
ldd /opt/sealien-joystick/bin/sealien-ctrlsmt-joystick-1.0.0
```

确保所有依赖库都已安装。

### 检查权限

确保用户有访问输入设备的权限：
```bash
# 检查用户是否在input组
groups | grep input

# 如果不在，添加用户到input组
sudo usermod -aG input $USER
# 需要重新登录生效
```

### 检查ROS2环境（如果使用ROS2）

```bash
# 检查ROS2是否安装
source /opt/ros/humble/setup.bash
ros2 --version
```

## 📝 版本信息

- **产品名称**: Sealien Joystick
- **版本**: 1.0.0
- **架构**: ARM64 (aarch64)
- **构建日期**: $(date +%Y-%m-%d)

## 📞 技术支持

如有问题，请联系技术支持。
README_EOF

# 11. 检查依赖库并生成依赖列表
print_info "检查依赖库..."
DEPENDENCIES_FILE="${RELEASE_DIR}/DEPENDENCIES.txt"
echo "依赖库列表:" > "${DEPENDENCIES_FILE}"
echo "===========" >> "${DEPENDENCIES_FILE}"
echo "" >> "${DEPENDENCIES_FILE}"
echo "C++ 可执行文件依赖:" >> "${DEPENDENCIES_FILE}"
ldd "${BUILD_DIR}/${EXECUTABLE_NAME}" 2>/dev/null | grep -v "linux-vdso\|ld-linux" | sed 's/^/  /' >> "${DEPENDENCIES_FILE}" || true
echo "" >> "${DEPENDENCIES_FILE}"
echo "需要安装的系统包:" >> "${DEPENDENCIES_FILE}"
echo "  - libmodbus5" >> "${DEPENDENCIES_FILE}"
echo "  - libyaml-cpp0.7" >> "${DEPENDENCIES_FILE}"
echo "  - python3-pip (网页控制面板需要)" >> "${DEPENDENCIES_FILE}"
echo "  - ros-humble-desktop (可选，如果使用ROS2)" >> "${DEPENDENCIES_FILE}"
if [ "$HAS_WEB_PANEL" = true ]; then
    echo "" >> "${DEPENDENCIES_FILE}"
    echo "Python 依赖 (网页控制面板):" >> "${DEPENDENCIES_FILE}"
    if [ -f "${WEB_PANEL_DIR}/requirements.txt" ]; then
        cat "${WEB_PANEL_DIR}/requirements.txt" | sed 's/^/  /' >> "${DEPENDENCIES_FILE}"
    fi
fi

# 12. 创建压缩包
print_info "创建压缩包..."
tar -czf "${RELEASE_DIR}.tar.gz" "${RELEASE_DIR}"

# 13. 显示包信息
print_success "部署包创建完成！"
echo ""
echo "📦 包信息:"
echo "  文件名: ${RELEASE_DIR}.tar.gz"
echo "  大小: $(du -h ${RELEASE_DIR}.tar.gz | cut -f1)"
echo ""
echo "📁 包内容:"
echo "   ├── bin/sealien-ctrlsmt-joystick-1.0.0  (可执行文件)"
echo "   ├── config/                              (配置文件目录)"
echo "   ├── scripts/run.sh                       (启动脚本)"
if [ "$HAS_WEB_PANEL" = true ]; then
    echo "   ├── web_panel/                         (网页控制面板)"
    echo "   │   ├── backend/                       (Python后端)"
    echo "   │   ├── frontend/                      (前端文件)"
    echo "   │   ├── scripts/start.sh               (启动脚本)"
    echo "   │   ├── config/                        (配置文件)"
    echo "   │   └── requirements.txt               (Python依赖)"
fi
echo "   ├── install.sh                           (安装脚本)"
echo "   ├── install_dependencies.sh               (依赖安装脚本)"
echo "   ├── README_DEPLOY.md                     (部署说明)"
echo "   └── DEPENDENCIES.txt                     (依赖列表)"
echo ""
echo "🚀 部署到 Orin NX 的步骤:"
echo "  1. 传输部署包到 Orin NX:"
echo "     scp ${RELEASE_DIR}.tar.gz user@orin-nx:/tmp/"
echo ""
echo "  2. 在 Orin NX 上解压:"
echo "     tar -xzf ${RELEASE_DIR}.tar.gz"
echo "     cd ${RELEASE_DIR}"
echo ""
echo "  3. 安装系统依赖（推荐使用脚本）:"
echo "     sudo ./install_dependencies.sh"
echo ""
echo "     或者手动安装:"
echo "     sudo apt update"
echo "     sudo apt install -y libmodbus5 libyaml-cpp0.7 python3-pip"
if [ "$HAS_WEB_PANEL" = true ]; then
    echo "     pip3 install websockets PyYAML"
fi
echo ""
echo "  4. 运行安装脚本:"
echo "     sudo ./install.sh"
echo ""
echo "  5. 运行程序:"
echo "     sealien-joystick"
if [ "$HAS_WEB_PANEL" = true ]; then
    echo ""
    echo "  6. 启动网页控制面板（可选）:"
    echo "     sealien-web-panel"
    echo "     然后访问: http://localhost:8080/frontend/web_dashboard.html"
fi
echo ""
print_success "完成！"







