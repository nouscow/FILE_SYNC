/**
 * @file logger.h
 * @brief 线程安全日志类，支持级别过滤和基于文件大小的滚动备份。
 *        静态单例模式：init() 初始化，get() 获取引用。
 */

#ifndef LOGGER_H
#define LOGGER_H
#include<string>

#include<fstream>
enum LogLevel{DEBUG=0,INFO=1,ERROR=2};
#include<mutex>
#include<chrono>
#include<memory>
class Logger{
private:
std::string file_path;
LogLevel level;
std::ofstream file_;
std::mutex mutex_;
static std::unique_ptr<Logger> logger;
// 日志滚动参数
long max_size_bytes = 10 * 1024 * 1024; // 默认 10MB
int backup_count = 3;                    // 默认保留 3 份

// 检查当前文件大小，超过 max_size_bytes 则滚动备份
void rotate();

public:
  // 构造：以追加模式打开日志文件
  Logger( std::string file_path,LogLevel level = LogLevel::INFO);

  // 初始化静态单例，设置滚动参数
  static void init(const std::string& file_path, long max_size_bytes = 10*1024*1024, int backup_count = 3);

  // 获取单例引用（必须先调用 init）
  static Logger& get();

  // 核心写入：级别过滤 → 加锁 → 滚动检查 → 写入文件
  void log(LogLevel lvl, const std::string& message);

  // 便捷方法：对应三个日志级别
  void debug(const std::string& msg);
  void info(const std::string& msg);
  void error(const std::string& msg);

  // 将 LogLevel 枚举转为字符串（DEBUG/INFO/ERROR）
  std::string getLeveStr(LogLevel level);

  // 获取当前时间戳，格式 YYYY-MM-DD HH:MM:SS
  std::string get_timestamp();

  ~Logger();
};


#endif // LOGGER_H
