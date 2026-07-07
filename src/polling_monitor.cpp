/**
 * @file polling_monitor.cpp
 * @brief 轮询监控器实现：定时休眠后触发回调。
 */

#include "polling_monitor.h"
#include "monitor.h"
#include <thread>
#include <string>
#include <iostream>
#include "logger.h"

// 构造：保存轮询间隔，传递回调给基类
polling_monitor::polling_monitor(int interval_secs, SyncCallback callback):Monitor(std::move(callback)){
    this->interval_secs=interval_secs;
}

// 启动：设置 running_ = true，创建后台线程执行 loop()
void polling_monitor::start() {
    if (running_)return;
    running_ = true;
    worker_ = std::thread(&Monitor::loop,this);
}

// 停止：设置 running_ = false，join 等待线程退出
void polling_monitor::stop(){
    running_ = false;
    if (worker_.joinable()) {
        Logger::get().info("成功停止线程");
        worker_.join();
    }
}

// 主循环：sleep interval_secs → 检查 running_ → 触发 callback
void polling_monitor::loop() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(this->interval_secs));
        if (running_ && callback) {
            callback();
        }
    }
}
