/**
 * @file inotify_monitor.h
 * @brief inotify 实时监控器(Linux)，文件系统事件驱动同步回调。
 */
#ifndef INOTIFY_MONITOR_H
#define INOTIFY_MONITOR_H
#include"monitor.h"

class inotify_monitor:public Monitor{
private:
    int _fd_;     // inotify_init() 返回的文件描述符
    int _wd_;     // inotify_add_watch() 返回的监视描述符
    std::string watch_dir;

public:
    inotify_monitor(std::string watch_dir, SyncCallback callback);

    // 初始化 inotify fd 和 watch，启动后台线程
    void start() override;

    // 移除 watch、关闭 fd，join 等待线程退出
    void stop() override;

    // 主循环：阻塞读取 inotify 事件，有事件则触发 callback
    void loop() override;

    // TODO: 精确过滤事件类型
    //       当前仅检查 event->len > 0，对目录的 IN_ACCESS 等无关事件也触发回调
    //       应只关注 IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_TO
};
#endif // INOTIFY_MONITOR_H
