/**
 * @file monitor.cpp
 * @brief Monitor 基类构造函数实现。
 */

#include "monitor.h"
#include <thread>
#include <string>
#include <iostream>

// 构造：保存回调函数，初始化 running_ 为 false
Monitor::Monitor(SyncCallback callback):callback(std::move(callback)),running_(false){
}
