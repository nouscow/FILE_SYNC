/**
 * @file monitor.cpp
 * @brief 轮询监控器的实现
 *
 * 实现要点：
 * - start() 创建一个新线程，在新线程中执行 loop()。
 * - loop() 循环：休眠 interval_secs 秒，检查 running_，若仍运行则调用回调。
 * - 回调中通常执行：扫描源目录 → 对比差异 → 同步文件。
 * - 使用 std::this_thread::sleep_for 实现休眠。
 * - 注意线程安全：回调中不应直接操作 Monitor 成员（除非加锁）。
 *
 * 第二阶段替换：
 * - 新建 InotifyMonitor 继承自 Monitor（或独立实现），使用 inotify API。
 * - 事件到达后立即触发同步，而非轮询。
 */

#include "monitor.h"
#include<thread>
#include<string>
#include<iostream>


Monitor::Monitor(SyncCallback callback):callback(std::move(callback)),running_(false){

}



