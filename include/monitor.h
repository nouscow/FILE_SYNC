/**
 * @file monitor.h
 * @brief 目录监控器抽象基类，通过回调函数驱动同步流程。
 */

#ifndef MONITOR_H
#define MONITOR_H
#include<functional>
#include<atomic>
#include<thread>
#include<string>
class Monitor{

protected:
using SyncCallback=std::function<void()>;
 SyncCallback callback;        // 同步回调函数
 std::atomic<bool> running_;   // 线程运行标志
 std::thread worker_;          // 后台工作线程

public:
 // 构造：保存回调，初始化 running_ 为 false
 Monitor(SyncCallback callback);

 // 启动后台线程执行 loop()
 virtual void start()=0;

 // 请求停止，等待线程退出
 virtual void stop()=0;

 // 线程主循环（由子类实现具体策略）
 virtual void loop()=0;

 virtual ~Monitor()=default;
};



#endif // MONITOR_H
