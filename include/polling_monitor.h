// polling_monitor.h（Windows）
#include"monitor.h"
class polling_monitor:public Monitor{
private:
    int interval_secs;//轮询间隔
public:
 
polling_monitor(int interval_secs, SyncCallback callback);//init
 void start() override;  // 启动后台线程
 void stop()override  ;  // 请求停止
 void loop()override;//线程主循环
};