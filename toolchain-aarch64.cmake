# CMake 交叉编译工具链文件 - ARM64 (aarch64)
# 用于在 x86_64 主机上为 Orin NX 编译

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# 交叉编译器
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# 设置 pkg-config 为交叉编译版本
set(PKG_CONFIG_EXECUTABLE aarch64-linux-gnu-pkg-config)

# 查找程序、库和包的模式
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# 设置目标架构优化
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a -mtune=cortex-a78")

# 设置交叉编译的库搜索路径（如果安装了 ARM64 开发库）
# 这些路径通常在 /usr/aarch64-linux-gnu/ 或 /usr/lib/aarch64-linux-gnu/
set(CMAKE_FIND_ROOT_PATH 
    /usr/aarch64-linux-gnu
    /usr/lib/aarch64-linux-gnu
)

message(STATUS "🔧 使用 ARM64 交叉编译工具链")
message(STATUS "   C 编译器: ${CMAKE_C_COMPILER}")
message(STATUS "   C++ 编译器: ${CMAKE_CXX_COMPILER}")
message(STATUS "   目标架构: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "   pkg-config: ${PKG_CONFIG_EXECUTABLE}")

