/**
 * @file logger.cpp
 * @brief 日志类实现：静态单例、级别过滤、文件大小滚动备份。
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
#include<filesystem>

// 静态成员定义
std::unique_ptr<Logger> Logger::logger = nullptr;

// 构造：以追加模式打开日志文件
Logger::Logger(std::string file_path, LogLevel lvl) :file_path(file_path), level(lvl) {
    file_.open(file_path, std::ios::app);
    if (!file_) {
        std::cout << "LOGGER OPEN ERROR" << std::endl;
    }
}

// 初始化静态单例：创建 Logger 实例并设置滚动参数
void Logger::init(const std::string& file_path, long max_size_bytes, int backup_count) {
    logger = std::make_unique<Logger>(file_path);
    logger->max_size_bytes = max_size_bytes;
    logger->backup_count = backup_count;
}

// 获取单例引用（必须先调用 init）
Logger& Logger::get() {
    return *logger;
}

// 滚动备份：检查文件大小 → 超限则关闭文件 → 依次后移备份 → 重开新文件
void Logger::rotate() {
    file_.flush();
    std::error_code ec;
    auto size = std::filesystem::file_size(file_path, ec);
    if (ec || size < static_cast<std::uintmax_t>(max_size_bytes)) {
        return;
    }

    file_.close();

    // 删除最旧备份，依次后移：.2→.3, .1→.2
    std::string oldest = file_path + "." + std::to_string(backup_count);
    std::filesystem::remove(oldest, ec);

    for (int i = backup_count - 1; i >= 1; --i) {
        std::string src = file_path + "." + std::to_string(i);
        std::string dst = file_path + "." + std::to_string(i + 1);
        std::filesystem::rename(src, dst, ec);
    }

    // 当前文件 → .1
    std::filesystem::rename(file_path, file_path + ".1", ec);

    file_.open(file_path, std::ios::app);
    if (!file_) {
        std::cout << "LOGGER OPEN ERROR after rotation" << std::endl;
    }
}

// 核心写入：级别过滤 → 加锁 → 滚动检查 → 格式化写入 → flush
void Logger::log(LogLevel lvl, const std::string& message) {
    if (lvl < this->level) return;
    std::lock_guard<std::mutex> lock(mutex_);

    rotate();

    if (file_.is_open()) {
        file_ << "[" << this->getLeveStr(lvl) << "][" << this->get_timestamp() << "][" << message << "]" << std::endl;
        file_.flush();
    }
}

// 将 LogLevel 枚举转为字符串
std::string Logger::getLeveStr(LogLevel lvl) {
    std::string levelstr;
    switch (lvl)
    {
    case 0:
        levelstr = "DEBUG";
        break;
    case 1:
        levelstr = "INFO";
        break;
    case 2:
        levelstr = "ERROR";
        break;
    default:
        levelstr = "UNKNOWN_ERROR";
        break;
    }
    return levelstr;
}

// 获取当前时间戳，格式 YYYY-MM-DD HH:MM:SS
// Windows 用 localtime_s()，Linux 用 localtime_r()
std::string Logger::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm bt;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&bt, &now_c);
#else
    localtime_r(&now_c, &bt);
#endif

    std::ostringstream oss;
    oss << std::put_time(&bt, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// 便捷方法：DEBUG 级别
void Logger::debug(const std::string& msg) {
    log(LogLevel::DEBUG, msg);
}

// 便捷方法：INFO 级别
void Logger::info(const std::string& msg) {
    log(LogLevel::INFO, msg);
}

// 便捷方法：ERROR 级别
void Logger::error(const std::string& msg) {
    log(LogLevel::ERROR, msg);
}

// 析构：关闭日志文件
Logger::~Logger() {
    file_.close();
}
