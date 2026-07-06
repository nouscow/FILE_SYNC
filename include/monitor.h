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

 *
 * 注意事项：
 * - start() 内部创建分离线程（std::thread::detach），需确保回调函数生命周期有效。
 * - 停止时设置 running_ = false，线程在下一次休眠结束后退出。
 * - 第二阶段将替换为 InotifyMonitor 子类，使用 inotify 实时监听。
 */

#ifndef MONITOR_H
#define MONITOR_H
#include<functional>
#include<atomic>
#include<thread>
#include<string>
class Monitor{

public:

using SyncCallback=std::function<void()>;


void start() ;  // 启动后台线程
void stop()  ;  // 请求停止


#ifdef _WIN32_||_WIN64_
public:
Monitor(int interval_secs, SyncCallback callback);
#else
public:
Monitor(int interval_secs,std::string watch_dir, SyncCallback callback);
private:
int _fd_;//=inotify_init
int _wd_;//=inotify_add_watch
std::string watch_dir;//
#endif
private:
    int interval_secs;//轮询间隔
    SyncCallback callback;//要调用的同步函数
    std::atomic<bool> running_;//线程运行标志
    std::thread worker_;
    
    void loop();//线程主循环

};


#endif // MONITOR_H
