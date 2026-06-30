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
// ... 实现 ...