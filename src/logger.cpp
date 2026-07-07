/**
 * @file logger.cpp
 * @brief 日志类的实现
 *
 * 实现要点：
 * - 静态单例模式：通过 init() 初始化，get() 获取引用。
 * - 构造函数以追加模式打开文件（std::ios::app）。
 * - 每次写入前检查文件大小，超过 max_size_bytes 则滚动。
 * - 滚动策略：当前文件重命名为 .1，旧的 .1 → .2，依此类推，超出 backup_count 的删除。
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
#include<filesystem>

// 静态成员定义
std::unique_ptr<Logger> Logger::logger = nullptr;

Logger::Logger(std::string file_path, LogLevel lvl) :file_path(file_path), level(lvl) {
    file_.open(file_path, std::ios::app);
    if (!file_) {
        std::cout << "LOGGER OPEN ERROR" << std::endl;
    }
}

void Logger::init(const std::string& file_path, long max_size_bytes, int backup_count) {
    logger = std::make_unique<Logger>(file_path);
    logger->max_size_bytes = max_size_bytes;
    logger->backup_count = backup_count;
}

Logger& Logger::get() {
    return *logger;
}

void Logger::rotate() {
    // 检查当前日志文件大小
    file_.flush();
    std::error_code ec;
    auto size = std::filesystem::file_size(file_path, ec);
    if (ec || size < static_cast<std::uintmax_t>(max_size_bytes)) {
        return;
    }

    // 关闭当前文件
    file_.close();

    // 滚动备份文件：删除最旧的，依次后移
    std::string oldest = file_path + "." + std::to_string(backup_count);
    std::filesystem::remove(oldest, ec);

    for (int i = backup_count - 1; i >= 1; --i) {
        std::string src = file_path + "." + std::to_string(i);
        std::string dst = file_path + "." + std::to_string(i + 1);
        std::filesystem::rename(src, dst, ec);
    }

    // 当前文件 → .1
    std::filesystem::rename(file_path, file_path + ".1", ec);

    // 重新打开新文件
    file_.open(file_path, std::ios::app);
    if (!file_) {
        std::cout << "LOGGER OPEN ERROR after rotation" << std::endl;
    }
}

void Logger::log(LogLevel lvl, const std::string& message) {
    if (lvl < this->level) return;
    std::lock_guard<std::mutex> lock(mutex_);

    // 写入前检查是否需要滚动
    rotate();

    if (file_.is_open()) {
        file_ << "[" << this->getLeveStr(lvl) << "][" << this->get_timestamp() << "][" << message << "]" << std::endl;
        file_.flush();
    }
}

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

void Logger::debug(const std::string& msg) {
    log(LogLevel::DEBUG, msg);
}

void Logger::info(const std::string& msg) {
    log(LogLevel::INFO, msg);
}

void Logger::error(const std::string& msg) {
    log(LogLevel::ERROR, msg);
}

Logger::~Logger() {
    file_.close();
}
