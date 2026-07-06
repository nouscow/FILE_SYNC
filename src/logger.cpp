/**
 * @file logger.cpp
 * @brief 日志类的实现
 *
 * 实现要点：
 * - 构造函数以追加模式打开文件（std::ios::app）。
 * - 获取当前时间戳，格式化输出。
 * - 按天滚动：每次写入前检查当前日期，若与上次不同则关闭旧文件、创建新文件。
 * - 使用 std::lock_guard<std::mutex> 保护文件写操作。
 * - 支持日志级别过滤：低于设定级别的消息不写入。
 *
 * 平台差异：
 * - Windows 下使用 localtime_s()，Linux 下使用 localtime_r()。
 */

#include "logger.h"
#include<fstream>
#include<iostream>
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>
#include<mutex>
#include<string>
Logger::Logger(std::string file_path, LogLevel level) :file_path(file_path), level(level) {

       file_.open(file_path,std::ios::app);
      if(!file_){
         std::cout<<"LOGGER OPEN ERROR"<<std::endl;
      }
      
   }
  void Logger::log(LogLevel level, const std::string& message){
      if (level < this->level)return;
      std::lock_guard<std::mutex>lock(mutex_);
      if (file_.is_open()) {

        
          file_ <<"["<<this->getLeveStr(level)<<"]["<<this->get_timestamp()<<"]["<< message <<"]"<< std::endl;
          file_.flush();
      }

  }
   std::string Logger::getLeveStr(LogLevel level){
    std::string levelstr;
        switch (level)
        {
        case 0:
        levelstr="DEBUG";
        break;
        case 1:
        levelstr="INFO";
        break;
        case 2:
         levelstr="ERROR";
        break;

        default:
          levelstr="UNKNOWN_ERROR";
            break;
        }
        return levelstr;
   }
  std::string Logger::get_timestamp() {
      // 获取当前系统时间点
      auto now = std::chrono::system_clock::now();
      std::time_t now_c = std::chrono::system_clock::to_time_t(now);

      // 转换为本地时间（线程安全版本）
      std::tm bt;
#if defined(_WIN32) || defined(_WIN64)
      localtime_s(&bt, &now_c);
#else
      localtime_r(&now_c, &bt);
#endif

      // 格式化为字符串
      std::ostringstream oss;
      oss << std::put_time(&bt, "%Y-%m-%d %H:%M:%S");
      return oss.str();
  }
   void Logger::debug(const std::string& msg){
       log(LogLevel::DEBUG, msg);
   }
   void Logger::info(const std::string& msg){
       log(LogLevel::INFO, msg);
   }
    void Logger::error(const std::string& msg){
        log(LogLevel::ERROR, msg);
    }
    Logger::~Logger(){
      file_.close();
    }
