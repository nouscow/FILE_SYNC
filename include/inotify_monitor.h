// inotify_monitor.h（Linux）
#include"monitor.h"
class inotify_monitor:public Monitor{

private:
int _fd_;//=inotify_init
int _wd_;//=inotify_add_watch
std::string watch_dir;//

public:
inotify_monitor(std::string watch_dir, SyncCallback callback);
 void start() override;  // 启动后台线程
void stop()override  ;  // 请求停止
void loop()override;//线程主循环
};