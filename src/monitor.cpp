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
#include"inotify_monitor.h"
#include"polling_monitor.h"
#ifdef __linux__
#include<sys/inotify.h>
#include<unistd.h>
#endif
Monitor::Monitor(SyncCallback callback):callback(std::move(callback)),running_(false){

}
polling_monitor::polling_monitor(int interval_secs, SyncCallback callback):interval_secs(interval_secs),Monitor(std::move(callback)){

}
void polling_monitor::start() {
	if (running_)return;
	running_ = true;
	worker_ = std::thread(&Monitor::loop,this);
}  // 启动后台线程
void polling_monitor::stop(){
	running_ = false;
	if (worker_.joinable()) {
		worker_.join();
	}
} // 请求停止
void polling_monitor::loop() {
	while (running_) {
		std::this_thread::sleep_for(std::chrono::seconds(this->interval_secs));
		if (running_ && callback) {
			callback();
		}
	}
}


inotify_monitor::inotify_monitor(std::string watch_dir,SyncCallback callback):watch_dir(watch_dir),Monitor(std::move(callback)){

}
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
}  // 启动后台线程
void inotify_monitor::stop(){
	running_ = false;

	if(_fd_>=0){
		int res=inotify_rm_watch(this->_fd_,this->_wd_);
		close(_fd_);
		this->_fd_=-1;
	}
	if (worker_.joinable()) {
		worker_.join();
	}
} // 请求停止
void inotify_monitor::loop() {   

	while (running_) {
		char buf[4096];
		struct inotify_event*event;
		int event_len=sizeof(struct inotify_event);
		int rd=read(this->_fd_,buf,sizeof(buf));
		int pos=0;
		while(rd>pos){
            event=(struct inotify_event*)(buf+pos);
			if(event->len>0){
				if (running_ && callback) {
			     callback();
		}
			}
			int upsize=event_len+event->len;
			pos+=upsize;

		}
		
	}
}

