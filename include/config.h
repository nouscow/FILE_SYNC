/**
 * @file config.h
 * @brief 配置结构体定义与 JSON 加载函数声明。
 */

#ifndef CONFIG_H
#define CONFIG_H
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
using json = nlohmann::json;

struct Config{
    std::string source_dir;                 // 源目录路径
    std::string target_dir;                 // 目标目录路径
    int sync_interval_secs = 5;             // 轮询间隔(秒)
    std::vector<std::string> exclude_patterns; // 排除规则(正则表达式)

    // 日志配置
    std::string log_level = "INFO";
    std::string log_file_path = "./logs/file_sync.log";
    int log_max_size_mb = 10;               // 单个日志文件最大大小(MB)
    int log_backup_count = 3;               // 滚动备份文件数量

    // 重试配置
    int max_retries = 3;
    int retry_delay_secs = 2;

    // TODO: CLI 命令行参数支持（--source、--target、--interval 等）
    //       可在此增加命令行覆盖字段，由 load_config() 中合并处理
};

// 从 JSON 文件加载配置，返回填充好的 Config 结构体
Config load_config(const std::string& path);

// 将 Config 关键字段打印到 stdout，用于启动时确认
void ConfigPrintf(const Config&config);

#endif // CONFIG_H
