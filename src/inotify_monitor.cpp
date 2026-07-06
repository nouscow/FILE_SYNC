#include"inotify_monitor.h"
#include "monitor.h"
#include<thread>
#include<string>
#include<iostream>
#include<sys/inotify.h>
#include<unistd.h>

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
