# 🚀 Sealien-CtrlSmt-Joystick Makefile
# 提供便捷的构建目标和项目管理

.PHONY: help all build clean clean-all reconfigure package test info install run debug release quiet

# 默认目标
.DEFAULT_GOAL := help

# 项目信息
PROJECT_NAME := Sealien-CtrlSmt-Joystick
PROJECT_VERSION := 1.0.0
BUILD_DIR := build

# 颜色定义
GREEN := \033[0;32m
YELLOW := \033[1;33m
BLUE := \033[0;34m
CYAN := \033[0;36m
NC := \033[0m # No Color

# 帮助信息
help: ## 显示帮助信息
	@echo -e "$(CYAN)🚀 $(PROJECT_NAME) v$(PROJECT_VERSION) Makefile$(NC)"
	@echo ""
	@echo -e "$(YELLOW)可用的目标:$(NC)"
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {printf "  $(GREEN)%-15s$(NC) %s\n", $$1, $$2}' $(MAKEFILE_LIST)
	@echo ""
	@echo -e "$(YELLOW)快速开始:$(NC)"
	@echo "  make build          # 构建项目"
	@echo "  make run            # 构建并运行"
	@echo "  make clean          # 清理构建"
	@echo "  make debug          # 调试模式构建"
	@echo "  make release        # 发布模式构建"
	@echo "  make quiet          # 安静构建 (最小化警告)"

# 主要目标
all: build ## 构建项目（默认）

build: ## 构建项目
	@echo -e "$(BLUE)🔨 构建项目...$(NC)"
	@if [ ! -d "$(BUILD_DIR)" ]; then mkdir -p "$(BUILD_DIR)"; fi
	@cd "$(BUILD_DIR)" && cmake .. && make -j$(shell nproc)
	@echo -e "$(GREEN)✅ 构建完成$(NC)"

build-arm64: ## 交叉编译 ARM64 版本（用于 Orin NX）
	@echo -e "$(BLUE)🔨 交叉编译 ARM64 版本...$(NC)"
	@echo -e "$(YELLOW)⚠️  需要安装交叉编译工具链:$(NC)"
	@echo -e "   sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu"
	@echo ""
	@if ! command -v aarch64-linux-gnu-gcc >/dev/null 2>&1; then \
		echo -e "$(RED)❌ 未找到 aarch64-linux-gnu-gcc，请先安装交叉编译工具链$(NC)"; \
		exit 1; \
	fi
	@echo -e "$(YELLOW)💡 提示: 首次编译可能需要下载 yaml-cpp 源码，请耐心等待...$(NC)"
	@echo ""
	@if [ ! -d "$(BUILD_DIR)" ]; then mkdir -p "$(BUILD_DIR)"; fi
	@cd "$(BUILD_DIR)" && \
		cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-aarch64.cmake && \
		make -j$(shell nproc)
	@echo -e "$(GREEN)✅ ARM64 交叉编译完成$(NC)"

# 清理目标
clean: ## 清理构建文件
	@echo -e "$(YELLOW)🧹 清理构建文件...$(NC)"
	@if [ -d "$(BUILD_DIR)" ]; then cd "$(BUILD_DIR)" && make clean; fi
	@echo -e "$(GREEN)✅ 清理完成$(NC)"

clean-all: ## 深度清理构建目录
	@echo -e "$(YELLOW)🧹 深度清理构建目录...$(NC)"
	@if [ -d "$(BUILD_DIR)" ]; then cd "$(BUILD_DIR)" && make clean-all; fi
	@echo -e "$(GREEN)✅ 深度清理完成$(NC)"

# 配置目标
reconfigure: ## 重新配置项目
	@echo -e "$(BLUE)🔄 重新配置项目...$(NC)"
	@if [ ! -d "$(BUILD_DIR)" ]; then mkdir -p "$(BUILD_DIR)"; fi
	@cd "$(BUILD_DIR)" && cmake ..
	@echo -e "$(GREEN)✅ 重新配置完成$(NC)"

# 构建模式
debug: ## 调试模式构建
	@echo -e "$(BLUE)🔍 调试模式构建...$(NC)"
	@if [ ! -d "$(BUILD_DIR)" ]; then mkdir -p "$(BUILD_DIR)"; fi
	@cd "$(BUILD_DIR)" && cmake .. -DENABLE_DEBUG=ON -DENABLE_OPTIMIZATION=OFF && make -j$(shell nproc)
	@echo -e "$(GREEN)✅ 调试模式构建完成$(NC)"

release: ## 发布模式构建
	@echo -e "$(BLUE)⚡ 发布模式构建...$(NC)"
	@if [ ! -d "$(BUILD_DIR)" ]; then mkdir -p "$(BUILD_DIR)"; fi
	@cd "$(BUILD_DIR)" && cmake .. -DENABLE_DEBUG=OFF -DENABLE_OPTIMIZATION=ON && make -j$(shell nproc)
	@echo -e "$(GREEN)✅ 发布模式构建完成$(NC)"

quiet: ## 安静构建 (最小化警告)
	@echo -e "$(BLUE)🔇 安静构建模式...$(NC)"
	@if [ ! -d "$(BUILD_DIR)" ]; then mkdir -p "$(BUILD_DIR)"; fi
	@cd "$(BUILD_DIR)" && cmake .. -DENABLE_QUIET_BUILD=ON && make -j$(shell nproc)
	@echo -e "$(GREEN)✅ 安静构建完成$(NC)"

# 特殊目标
package: ## 创建部署包
	@echo -e "$(BLUE)📦 创建部署包...$(NC)"
	@if [ -d "$(BUILD_DIR)" ]; then cd "$(BUILD_DIR)" && make package; fi

test: ## 测试构建输出
	@echo -e "$(BLUE)🧪 测试构建输出...$(NC)"
	@if [ -d "$(BUILD_DIR)" ]; then cd "$(BUILD_DIR)" && make test-build; fi



info: ## 显示项目信息
	@echo -e "$(BLUE)📊 显示项目信息...$(NC)"
	@if [ -d "$(BUILD_DIR)" ]; then cd "$(BUILD_DIR)" && make info; fi

install: ## 安装到系统
	@echo -e "$(BLUE)📥 安装到系统...$(NC)"
	@if [ -d "$(BUILD_DIR)" ]; then cd "$(BUILD_DIR)" && sudo make install; fi

# 运行目标
run: build ## 构建并运行程序
	@echo -e "$(BLUE)🚀 运行程序...$(NC)"
	@if [ -f "$(BUILD_DIR)/sealien-ctrlsmt-joystick" ]; then \
		cd "$(BUILD_DIR)" && ./sealien-ctrlsmt-joystick ../config/AMSAMOTION_IO8R.yaml; \
	else \
		echo -e "$(YELLOW)⚠️  请先构建项目: make build$(NC)"; \
	fi

# 开发目标
dev: debug ## 开发模式构建（调试模式）
	@echo -e "$(GREEN)🔧 开发模式构建完成$(NC)"

prod: release ## 生产模式构建（发布模式）
	@echo -e "$(GREEN)🚀 生产模式构建完成$(NC)"

# 状态检查
status: ## 检查构建状态
	@echo -e "$(CYAN)📊 构建状态检查$(NC)"
	@if [ -d "$(BUILD_DIR)" ]; then \
		echo "构建目录: $(BUILD_DIR)"; \
		if [ -f "$(BUILD_DIR)/sealien-ctrlsmt-joystick" ]; then \
			echo "可执行文件: ✅ 存在"; \
			echo "文件大小: $$(ls -lh "$(BUILD_DIR)/sealien-ctrlsmt-joystick" | awk '{print $$5}')"; \
			echo "构建时间: $$(stat -c %y "$(BUILD_DIR)/sealien-ctrlsmt-joystick")"; \
		else \
			echo "可执行文件: ❌ 不存在"; \
		fi; \
	else \
		echo "构建目录: ❌ 不存在"; \
	fi

# 依赖检查
deps: ## 检查系统依赖
	@echo -e "$(CYAN)🔍 检查系统依赖$(NC)"
	@echo "CMake: $$(which cmake 2>/dev/null || echo '❌ 未安装')"
	@echo "GCC: $$(which gcc 2>/dev/null || echo '❌ 未安装')"
	@echo "Make: $$(which make 2>/dev/null || echo '❌ 未安装')"
	@echo "libmodbus: $$(pkg-config --exists libmodbus && echo '✅ 已安装' || echo '❌ 未安装')"
	@echo "yaml-cpp: $$(pkg-config --exists yaml-cpp && echo '✅ 已安装' || echo '❌ 未安装')"
	@echo "nlohmann_json: $$(pkg-config --exists nlohmann_json && echo '✅ 已安装' || echo '❌ 未安装')"

# 快速清理和重建
rebuild: clean-all build ## 完全重建项目

# 显示帮助（当没有指定目标时）
%:
	@echo -e "$(YELLOW)⚠️  未知目标: $@$(NC)"
	@echo "使用 'make help' 查看可用目标"
