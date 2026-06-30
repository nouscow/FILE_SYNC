/**
 * @file logger.h
 * @brief 线程安全的日志类声明
 *
 * 功能：
 * - 支持三种日志级别：DEBUG、INFO、ERROR。
 * - 日志输出到文件，按天滚动（每日新文件）。
 * - 所有写操作加互斥锁，保证多线程安全。
 * - 提供便捷方法：debug(), info(), error()。
 *
 * 接口：
 * - Logger(const std::string& file_path, LogLevel level = LogLevel::INFO)
 * - void log(LogLevel level, const std::string& message)
 * - void debug(const std::string& msg)
 * - void info(const std::string& msg)
 * - void error(const std::string& msg)
 *
 * 注意事项：
 * - 析构函数自动关闭文件。
 * - 日志文件路径中的目录需预先创建，或由 Logger 自动创建。
 */

#ifndef LOGGER_H
#define LOGGER_H
#include<string>
enum LogLevel{DEBUG=0,INFO=1,ERROR=2};
class Logger{
private:
const std::string& file_path; 
LogLevel level;

public:
  Logger(const std::string& file_path,LogLevel level = LogLevel::INFO);
  void log(LogLevel level, const std::string& message);
   void debug(const std::string& msg);
   void info(const std::string& msg);
    void error(const std::string& msg);
};


#endif // LOGGER_H
