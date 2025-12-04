cmake_minimum_required(VERSION 3.16)

# 禁用测试（除非明确启用）
option(ENABLE_TESTING "Enable testing (CTest)" OFF)
if(NOT ENABLE_TESTING)
    set(BUILD_TESTING OFF)
endif()

project(Sealien-CtrlSmt-Joystick CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置项目版本
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# 编译选项配置
option(ENABLE_DEBUG "Enable debug mode" OFF)
option(ENABLE_OPTIMIZATION "Enable optimization" ON)
option(ENABLE_WARNINGS "Enable warnings" ON)
option(ENABLE_QUIET_BUILD "Enable quiet build (minimal warnings)" OFF)
option(ENABLE_ROS2 "Enable ROS2 support" ON)

# 编译选项：根据配置设置不同的编译标志
if(ENABLE_DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -DDEBUG")
    message(STATUS "🔍 Debug mode enabled")
else()
    set(CMAKE_BUILD_TYPE Release)
    if(ENABLE_OPTIMIZATION)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
        message(STATUS "⚡ Release mode with optimization enabled")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -DNDEBUG")
        message(STATUS "📦 Release mode without optimization")
    endif()
endif()

# 警告设置
if(ENABLE_QUIET_BUILD)
    # 安静构建模式：只显示错误，最小化警告
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
    message(STATUS "🔇 Quiet build mode enabled (minimal warnings)")
elseif(ENABLE_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wunused-parameter")
        # 禁用一些过于严格的警告
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-conversion -Wno-sign-conversion -Wno-float-conversion")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wunused-parameter")
        # 禁用一些过于严格的警告
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-conversion -Wno-sign-conversion -Wno-float-conversion")
    endif()
    message(STATUS "⚠️  Warnings enabled (optimized)")
else()
    # 默认模式：只显示重要警告
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    message(STATUS "📝 Default warnings enabled")
endif()

# 架构优化
if(ENABLE_OPTIMIZATION)
    # 如果设置了交叉编译目标架构，使用目标架构的优化
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a -mtune=cortex-a78")
        message(STATUS "🏗️  Architecture optimization enabled for ARM64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native")
        message(STATUS "🏗️  Architecture optimization enabled for x86_64")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native")
        message(STATUS "🏗️  Architecture optimization enabled")
    endif()
endif()

# 编译器特定的优化和警告控制
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC特定优化
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
    # 禁用一些常见的假阳性警告
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized -Wno-unused-but-set-variable")
    message(STATUS "🔧 GCC-specific optimizations enabled")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang特定优化
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
    # 禁用一些常见的假阳性警告
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
    message(STATUS "🔧 Clang-specific optimizations enabled")
endif()

# 查找依赖包
find_package(PkgConfig REQUIRED)

# 设置 pkg-config 路径（交叉编译时）
if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(ENV{PKG_CONFIG_PATH} "/usr/lib/aarch64-linux-gnu/pkgconfig:/usr/share/aarch64-linux-gnu/pkgconfig:$ENV{PKG_CONFIG_PATH}")
    set(ENV{PKG_CONFIG_LIBDIR} "/usr/lib/aarch64-linux-gnu/pkgconfig:/usr/share/aarch64-linux-gnu/pkgconfig")
    set(ENV{PKG_CONFIG_SYSROOT_DIR} "/usr/aarch64-linux-gnu")
endif()

pkg_check_modules(LIBMODBUS REQUIRED libmodbus)

# 对于 yaml-cpp，交叉编译时从源码编译
if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    # 交叉编译时，尝试使用 pkg-config 或手动指定路径
    pkg_check_modules(YAML_CPP QUIET yaml-cpp)
    if(YAML_CPP_FOUND)
        message(STATUS "找到 yaml-cpp (通过 pkg-config): ${YAML_CPP_VERSION}")
        set(yaml-cpp_FOUND TRUE)
        set(yaml-cpp_INCLUDE_DIRS ${YAML_CPP_INCLUDE_DIRS})
        set(yaml-cpp_LIBRARIES ${YAML_CPP_LIBRARIES})
    else()
        # 尝试在标准交叉编译路径中查找
        find_path(yaml-cpp_INCLUDE_DIR yaml-cpp/yaml.h
            PATHS /usr/aarch64-linux-gnu/include
                  /usr/include/aarch64-linux-gnu
        )
        find_library(yaml-cpp_LIBRARY yaml-cpp
            PATHS /usr/aarch64-linux-gnu/lib
                  /usr/lib/aarch64-linux-gnu
        )
        if(yaml-cpp_INCLUDE_DIR AND yaml-cpp_LIBRARY)
            set(yaml-cpp_FOUND TRUE)
            set(yaml-cpp_INCLUDE_DIRS ${yaml-cpp_INCLUDE_DIR})
            set(yaml-cpp_LIBRARIES ${yaml-cpp_LIBRARY})
            message(STATUS "找到 yaml-cpp (手动查找): ${yaml-cpp_LIBRARY}")
        else()
            # 如果找不到，从源码编译
            message(STATUS "未找到预编译的 yaml-cpp，将从源码编译...")
            message(STATUS "这可能需要几分钟时间，请耐心等待...")
            include(FetchContent)
            # 设置下载和更新策略
            set(FETCHCONTENT_QUIET OFF)
            FetchContent_Declare(
                yaml-cpp
                GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
                GIT_TAG yaml-cpp-0.7.0
                GIT_SHALLOW TRUE  # 只下载最新的提交，加快下载速度
            )
            # 禁用 yaml-cpp 的测试和工具，加快编译速度
            set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
            set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
            set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
            FetchContent_MakeAvailable(yaml-cpp)
            set(yaml-cpp_FOUND TRUE)
            set(yaml-cpp_INCLUDE_DIRS ${yaml-cpp_SOURCE_DIR}/include)
            set(yaml-cpp_LIBRARIES yaml-cpp)
            message(STATUS "✓ yaml-cpp 已从源码编译完成")
        endif()
    endif()
else()
    find_package(yaml-cpp REQUIRED)
endif()

# nlohmann_json 通常是 header-only，交叉编译时可能需要从源码获取
if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    find_package(nlohmann_json QUIET)
    if(NOT nlohmann_json_FOUND)
        # 如果找不到，使用 FetchContent 从源码获取
        message(STATUS "未找到 nlohmann_json，将从源码获取...")
        include(FetchContent)
        FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.2
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(nlohmann_json)
        message(STATUS "✓ nlohmann_json 已从源码获取")
    endif()
else()
    find_package(nlohmann_json REQUIRED)
endif()

# 查找ROS2包（可选）
if(ENABLE_ROS2)
    find_package(ament_cmake QUIET)
    find_package(rclcpp QUIET)
    find_package(std_msgs QUIET)
    find_package(sensor_msgs QUIET)
    find_package(geometry_msgs QUIET)
endif()

# 创建主可执行文件
add_executable(sealien-ctrlsmt-joystick
  # 核心应用控制
  src/core/main.cpp
  src/core/application_controller.cpp
  
  # 配置管理模块
  src/config/config.cpp
  src/config/smart_config.cpp
  src/config/usb_vendor_config.cpp
  
  # 设备检测模块
  src/detection/usb_serial_detector.cpp
  src/detection/smart_serial_detector.cpp
  src/detection/joystick_detector.cpp
  
  # 通信模块
  src/communication/joystick/joystick_manager.cpp
  src/communication/modbus/modbus_client.cpp
  src/communication/modbus/modbus_data_processor.cpp
  src/communication/modbus/valve_control_processor.cpp
  
  # 输入处理模块
  src/input/joystick_mapping.cpp
  src/input/uinput_device.cpp
  
  # 工具模块
  src/utils/filters.cpp
  src/utils/mapping.cpp
  src/utils/utils.cpp
  src/utils/debug_config.cpp
)



# 设置主可执行文件属性
set_target_properties(sealien-ctrlsmt-joystick PROPERTIES
    OUTPUT_NAME "sealien-ctrlsmt-joystick"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)



# 添加编译定义
target_compile_definitions(sealien-ctrlsmt-joystick PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_NAME="${PROJECT_NAME}"
    $<$<CONFIG:Debug>:ENABLE_DEBUG_LOGS=1>
    $<$<CONFIG:Release>:ENABLE_DEBUG_LOGS=0>
)



# 设置包含目录
target_include_directories(sealien-ctrlsmt-joystick PRIVATE
  include
  include/core
  include/config
  include/detection
  include/communication
  include/communication/modbus
  include/communication/joystick
  include/input
  include/utils
  ${LIBMODBUS_INCLUDE_DIRS}
)



# 设置链接目录
target_link_directories(sealien-ctrlsmt-joystick PRIVATE ${LIBMODBUS_LIBRARY_DIRS})


# 链接基础库
if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    # 交叉编译时
    if(yaml-cpp_LIBRARIES)
        # 使用手动找到的库或从源码编译的库
        target_link_libraries(sealien-ctrlsmt-joystick PRIVATE
          ${LIBMODBUS_LIBRARIES}
          ${yaml-cpp_LIBRARIES}
          nlohmann_json::nlohmann_json
        )
        if(yaml-cpp_INCLUDE_DIRS)
            target_include_directories(sealien-ctrlsmt-joystick PRIVATE ${yaml-cpp_INCLUDE_DIRS})
        endif()
    else()
        # 如果使用 FetchContent，直接链接目标
        target_link_libraries(sealien-ctrlsmt-joystick PRIVATE
          ${LIBMODBUS_LIBRARIES}
          yaml-cpp
          nlohmann_json::nlohmann_json
        )
    endif()
else()
    # 正常编译时使用 find_package 的结果
    target_link_libraries(sealien-ctrlsmt-joystick PRIVATE
      ${LIBMODBUS_LIBRARIES}
      yaml-cpp
      nlohmann_json::nlohmann_json
    )
endif()



# 如果找到ROS2，则链接ROS2库
if(ENABLE_ROS2 AND rclcpp_FOUND)
  target_link_libraries(sealien-ctrlsmt-joystick PRIVATE
    rclcpp::rclcpp
    std_msgs::std_msgs__rosidl_typesupport_cpp
    sensor_msgs::sensor_msgs__rosidl_typesupport_cpp
    geometry_msgs::geometry_msgs__rosidl_typesupport_cpp
  )
  target_compile_definitions(sealien-ctrlsmt-joystick PRIVATE ENABLE_ROS2=1)
  message(STATUS "🤖 ROS2 support enabled with message types")
else()
  message(STATUS "❌ ROS2 not found or disabled, building without ROS2 support")
endif()

# 安装配置
install(TARGETS sealien-ctrlsmt-joystick
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装配置文件
install(DIRECTORY config/ DESTINATION share/${PROJECT_NAME}/config)

# 安装服务文件
install(FILES sealien-ctrlsmt-joystick.service DESTINATION lib/systemd/system)

# 安装文档
install(FILES README.md DESTINATION share/${PROJECT_NAME})

# 添加自定义目标

## 🧹 清理目标
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CTestTestfile.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CTestCustom.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CTestConfiguration.ini
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/ament_cmake_core
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/ament_cmake_index
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/ament_cmake_environment_hooks
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/ament_cmake_package_templates
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/ament_cmake_uninstall_target
    COMMAND ${CMAKE_COMMAND} -E echo "🧹 Build directory cleaned successfully!"
    COMMAND ${CMAKE_COMMAND} -E echo "💡 Run 'cmake .. && make' to rebuild"
    COMMENT "🧹 Deep cleaning build directory..."
)

## 🔄 重新配置目标
add_custom_target(reconfigure
    COMMAND ${CMAKE_COMMAND} -E echo "🔄 Reconfiguring project..."
    COMMAND ${CMAKE_COMMAND} ..
    COMMAND ${CMAKE_COMMAND} -E echo "✅ Reconfiguration complete!"
    COMMENT "🔄 Reconfiguring project..."
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

## 📦 打包目标
add_custom_target(package
    COMMAND ${CMAKE_COMMAND} -E echo "📦 Creating package..."
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/package
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/sealien-ctrlsmt-joystick ${CMAKE_BINARY_DIR}/package/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/config ${CMAKE_BINARY_DIR}/package/config
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_BINARY_DIR}/package/
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/sealien-ctrlsmt-joystick.service ${CMAKE_BINARY_DIR}/package/
    COMMAND ${CMAKE_COMMAND} -E echo "📦 Package created in ${CMAKE_BINARY_DIR}/package/"
    COMMENT "📦 Creating deployment package..."
)

## 🧪 测试目标
add_custom_target(test-build
    COMMAND ${CMAKE_COMMAND} -E echo "🧪 Testing build..."
    COMMAND ${CMAKE_COMMAND} -E echo "📁 Binary location: ${CMAKE_BINARY_DIR}/sealien-ctrlsmt-joystick"
    COMMAND ${CMAKE_COMMAND} -E echo "📏 Binary size:"
    COMMAND ls -lh ${CMAKE_BINARY_DIR}/sealien-ctrlsmt-joystick
    COMMAND ${CMAKE_COMMAND} -E echo "🔍 Dependencies:"
    COMMAND ldd ${CMAKE_BINARY_DIR}/sealien-ctrlsmt-joystick
    COMMAND ${CMAKE_COMMAND} -E echo "✅ Build test complete!"
    COMMENT "🧪 Testing build output..."
)

## 📊 信息目标
add_custom_target(info
    COMMAND ${CMAKE_COMMAND} -E echo "📊 Project Information:"
    COMMAND ${CMAKE_COMMAND} -E echo "   Name: ${PROJECT_NAME}"
    COMMAND ${CMAKE_COMMAND} -E echo "   Version: ${PROJECT_VERSION}"
    COMMAND ${CMAKE_COMMAND} -E echo "   Build Type: ${CMAKE_BUILD_TYPE}"
    COMMAND ${CMAKE_COMMAND} -E echo "   C++ Standard: ${CMAKE_CXX_STANDARD}"
    COMMAND ${CMAKE_COMMAND} -E echo "   Compiler: ${CMAKE_CXX_COMPILER_ID}"
    COMMAND ${CMAKE_COMMAND} -E echo "   Build Directory: ${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "   Source Directory: ${CMAKE_SOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "   ROS2 Support: $<IF:$<BOOL:${rclcpp_FOUND}>,Enabled,Disabled>"
    COMMENT "📊 Displaying project information..."
)

# 显示构建信息
message(STATUS "🚀 Building ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "🔧 Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "📁 Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "📁 Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "⚙️  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "🔨 Compiler: ${CMAKE_CXX_COMPILER_ID}")

# 显示可用的目标
message(STATUS "🎯 Available targets:")
message(STATUS "   make                    - Build the project")
message(STATUS "   make clean              - Clean build files")
message(STATUS "   make clean-all          - Deep clean build directory")
message(STATUS "   make reconfigure        - Reconfigure project")
message(STATUS "   make package            - Create deployment package")
message(STATUS "   make test-build         - Test build output")
message(STATUS "   make info               - Show project information")
message(STATUS "   make install            - Install to system")
