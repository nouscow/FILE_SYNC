#include"polling_monitor.h"
#include "monitor.h"
#include<thread>
#include<string>
#include<iostream>

polling_monitor::polling_monitor(int interval_secs, SyncCallback callback):Monitor(std::move(callback)){
  this->interval_secs=interval_secs;

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