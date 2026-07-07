/**
 * @file polling_monitor.h
 * @brief 轮询模式监控器(Windows/macOS)，定时触发同步回调。
 */
#ifndef POLLING_MONITOR_H
#define POLLING_MONITOR_H
#include"monitor.h"
class polling_monitor:public Monitor{
private:
    int interval_secs; // 轮询间隔(秒)
public:
    polling_monitor(int interval_secs, SyncCallback callback);

    // 启动后台线程，设置 running_ = true
    void start() override;

    // 设置 running_ = false，join 等待线程退出
    void stop() override;

    // 主循环：sleep → 检查 running_ → 调用 callback
    void loop() override;
};
#endif // POLLING_MONITOR_H
