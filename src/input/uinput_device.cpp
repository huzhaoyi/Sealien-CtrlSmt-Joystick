/*
 * @Author: Joey hu418@163.com
 * @Date: 2025-08-27 11:30:06
 * @LastEditors: Joey.hu hu418@163.com
 * @LastEditTime: 2025-08-27 11:30:06
 * @FilePath: src/uinput_device.cpp
 * @Description: Linux uinput虚拟输入设备实现
 * 创建虚拟摇杆设备，支持轴和按键输入，通过uinput接口与内核交互
 * Code By SRS-HUZY Compile
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 * Copyright (c) 2025 by Sealien Robotics, All Rights Reserved.
 */

#include "uinput_device.h"
#include "utils.h"
#include <linux/uinput.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <cstring>
#include <cerrno>

/**
 * @brief 析构函数：销毁虚拟设备
 */
UInputDevice::~UInputDevice(){
    destroy();
}

/**
 * @brief 创建虚拟输入设备
 * @param name 设备名称
 * @param vendor 厂商ID
 * @param product 产品ID
 * @param version 版本号
 * @param abs_codes 支持的轴代码数组
 * @param key_codes 支持的按键代码数组
 * @return 创建成功返回true，失败返回false
 */
bool UInputDevice::create(const std::string& name,
                          uint16_t vendor, uint16_t product, uint16_t version,
                          const std::vector<int>& abs_codes,
                          const std::vector<int>& key_codes) {
    // 打开uinput设备文件
    fd_ = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd_ < 0) {
        int err = errno;
        if (err == ENOENT) {
            DEBUG_UINPUT_ERROR("open /dev/uinput failed: 设备不存在");
            DEBUG_UINPUT_ERROR("可能原因: 1) uinput模块未加载 (运行 'sudo modprobe uinput')");
            DEBUG_UINPUT_ERROR("         2) Tegra/Jetson系统内核未编译uinput支持 (CONFIG_INPUT_UINPUT未启用)");
            DEBUG_UINPUT_ERROR("解决方案: 如果系统不支持uinput，可以启用ROS2功能，程序将仅使用ROS2输出");
        } else if (err == EACCES || err == EPERM) {
            DEBUG_UINPUT_ERROR("open /dev/uinput failed: 权限不足");
            DEBUG_UINPUT_ERROR("修复建议: 1) sudo chmod 666 /dev/uinput");
            DEBUG_UINPUT_ERROR("         2) 添加udev规则: echo 'KERNEL==\"uinput\", MODE=\"0666\"' | sudo tee /etc/udev/rules.d/99-uinput.rules");
        } else {
            DEBUG_UINPUT_ERROR("open /dev/uinput failed: " << strerror(err) << " (errno=" << err << ")");
        }
        DEBUG_UINPUT_ERROR("注意: 如果系统不支持uinput（如Tegra系统），可以启用ROS2功能继续运行");
        return false;
    }

    // 设置设备支持的事件类型
    ioctl(fd_, UI_SET_EVBIT, EV_KEY);    // 按键事件
    ioctl(fd_, UI_SET_EVBIT, EV_ABS);    // 轴事件

    // 注册支持的轴代码
    for (int c : abs_codes) ioctl(fd_, UI_SET_ABSBIT, c);
    // 注册支持的按键代码
    for (int b : key_codes) ioctl(fd_, UI_SET_KEYBIT, b);

    // 配置设备信息
    uinput_setup us{};
    us.id.bustype = BUS_USB;              // 总线类型：USB
    us.id.vendor  = vendor;               // 厂商ID
    us.id.product = product;              // 产品ID
    us.id.version = version;              // 版本号
    
    // 设置设备名称
    std::snprintf(us.name, UINPUT_MAX_NAME_SIZE, "%s", name.c_str());
    
    // 应用设备设置
    if (ioctl(fd_, UI_DEV_SETUP, &us) < 0) {
        DEBUG_UINPUT_ERROR("UI_DEV_SETUP failed");
        close(fd_); fd_=-1; return false;
    }

    // 创建虚拟设备（轴的范围稍后通过setAbsRange设置）
    if (ioctl(fd_, UI_DEV_CREATE) < 0) {
        DEBUG_UINPUT_ERROR("UI_DEV_CREATE failed");
        close(fd_); fd_=-1; return false;
    }
    return true;
}

/**
 * @brief 设置轴的取值范围和属性
 * @param code 轴代码
 * @param minv 最小值
 * @param maxv 最大值
 * @param fuzz 模糊值
 * @param flat 平坦值
 * @param resolution 分辨率
 */
void UInputDevice::setAbsRange(int code, int minv, int maxv, int fuzz, int flat, int resolution){
    if (fd_ < 0) return;
    
    uinput_abs_setup s{}; 
    s.code=code;
    s.absinfo.minimum=minv;      // 最小值
    s.absinfo.maximum=maxv;      // 最大值
    s.absinfo.value=0;           // 初始值
    s.absinfo.flat=flat;         // 平坦值
    s.absinfo.fuzz=fuzz;         // 模糊值
    s.absinfo.resolution=resolution; // 分辨率
    
    if (ioctl(fd_, UI_ABS_SETUP, &s) < 0) {
        DEBUG_UINPUT_ERROR("UI_ABS_SETUP failed for code=" << code);
    }
}

/**
 * @brief 发送轴事件
 * @param code 轴代码
 * @param value 轴值
 */
void UInputDevice::sendAbs(int code, int value){
    if (fd_ < 0) {
        DEBUG_UINPUT_ERROR("UInput 设备未初始化，无法发送轴事件");
        return;
    }
    
    input_event ev{}; 
    ev.type=EV_ABS;      // 事件类型：轴
    ev.code=code;         // 轴代码
    ev.value=value;       // 轴值
    
    ssize_t wr = write(fd_, &ev, sizeof(ev));
    (void)wr;
}

/**
 * @brief 发送按键事件
 * @param code 按键代码
 * @param value 按键状态（0=释放，1=按下，2=重复）
 */
void UInputDevice::sendKey(int code, int value){
    if (fd_ < 0) {
        DEBUG_UINPUT_ERROR("UInput device not initialized, cannot send key event");
        return;
    }
    
    input_event ev{}; 
    ev.type=EV_KEY;       // 事件类型：按键
    ev.code=code;         // 按键代码
    ev.value=value;       // 按键状态
    
    ssize_t wr = write(fd_, &ev, sizeof(ev));
    (void)wr;
}

/**
 * @brief 同步所有输入事件
 * 通知内核处理之前发送的所有事件
 */
void UInputDevice::sync(){
    if (fd_ < 0) {
        DEBUG_UINPUT_ERROR("UInput device not initialized, cannot sync events");
        return;
    }
    
    input_event ev{}; 
    ev.type=EV_SYN;       // 事件类型：同步
    ev.code=SYN_REPORT;   // 同步代码：报告
    ev.value=0;           // 同步值
    
    ssize_t wr = write(fd_, &ev, sizeof(ev));
    (void)wr;
}

/**
 * @brief 销毁虚拟设备
 * 关闭文件描述符并通知内核销毁设备
 */
void UInputDevice::destroy(){
    if (fd_ >= 0) {
        ioctl(fd_, UI_DEV_DESTROY);  // 通知内核销毁设备
        close(fd_);                   // 关闭文件描述符
        fd_ = -1;                     // 标记为无效
    }
}
