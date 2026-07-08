/**
 * @file inotify_monitor.cpp
 * @brief inotify 实时监控器实现(Linux)：读取 inotify 事件驱动同步回调。
 */

#ifdef __linux__
#include "inotify_monitor.h"
#include "monitor.h"
#include"logger.h"
#include <thread>
#include <string>
#include <iostream>
#include <sys/inotify.h>
#include <unistd.h>
#include<poll.h>
// 构造：保存监视目录路径，传递回调给基类
inotify_monitor::inotify_monitor(std::string watch_dir,SyncCallback callback):watch_dir(watch_dir),Monitor(std::move(callback)){
}

// 启动：初始化 inotify fd → 注册 watch → 启动后台线程
void inotify_monitor::start() {
    if (running_)return;
    running_ = true;
    this->_fd_=inotify_init();
    if(_fd_==-1){
        running_=false;
        return;
    }
    this->_wd_=inotify_add_watch(this->_fd_,this->watch_dir.c_str(),IN_ALL_EVENTS);
    if(this->_wd_==-1){
        running_=false;
        return;
    }
    worker_ = std::thread(&Monitor::loop,this);
}

// 停止：移除 watch → 关闭 fd → join 等待线程退出
void inotify_monitor::stop(){
    running_ = false;
    if(_fd_>=0){
        inotify_rm_watch(this->_fd_,this->_wd_);
        close(_fd_);
        this->_fd_=-1;
    }
    if (worker_.joinable()) {
        worker_.join();
    }
}

// 主循环：阻塞读取 inotify 事件，解析 inotify_event 结构体，
// 仅对 IN_CREATE / IN_MODIFY / IN_DELETE / IN_MOVED_TO 事件触发 callback
void inotify_monitor::loop() {
    char buf[4096];
    struct pollfd pfd;
    pfd.fd=this->_fd_;
    pfd.events=POLLIN;
    while (running_) {
        int ret=poll(&pfd,1,500);
        if(ret<0){
            Logger::get().error("poll error"+__LINE__);

            if(errno==EINTR)continue;

            break;
        }
        if(ret==0){
            Logger::get().debug("time out");
            continue;
        }
        if(pfd.revents&POLLIN){
             int rd=read(this->_fd_,buf,sizeof(buf));
              struct inotify_event*event;
             int event_len=sizeof(struct inotify_event);
      
          int pos=0;
           while(rd>pos){
            event=(struct inotify_event*)(buf+pos);
            if(event->len > 0 && (event->mask & (IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_TO))){
                if (running_ && callback) {
                    callback();
                }
            }
            int upsize=event_len+event->len;
            pos+=upsize;
            }
        }
       
       
    }
}
#endif
