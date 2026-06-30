/**
 * @file monitor.h
 * @brief 目录监控器声明（轮询模式）
 *
 * 功能：
 * - 每隔指定秒数调用回调函数，触发同步流程。
 * - 支持启动/停止，使用原子布尔标志控制循环。
 * - 回调函数类型为 std::function<void()>。
 *
 * 接口：
 * - Monitor(int interval_secs, SyncCallback callback)
 * - void start()   // 启动后台线程
 * - void stop()    // 请求停止
 * - bool is_running() const
 *
 * 注意事项：
 * - start() 内部创建分离线程（std::thread::detach），需确保回调函数生命周期有效。
 * - 停止时设置 running_ = false，线程在下一次休眠结束后退出。
 * - 第二阶段将替换为 InotifyMonitor 子类，使用 inotify 实时监听。
 */

#ifndef MONITOR_H
#define MONITOR_H
#include<functional>
class Monitor{
    int interval_secs;
public:
using SyncCallback=std::function<void()>;
Monitor(int interval_secs, SyncCallback callback);
void start() ;  // 启动后台线程
void stop()  ;  // 请求停止
bool is_running() const;
};


#endif // MONITOR_H
// ... 类声明 ...