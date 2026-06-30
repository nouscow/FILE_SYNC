/**
 * @file config.cpp
 * @brief 配置加载函数的实现
 *
 * 实现细节：
 * - 打开指定的 JSON 文件（默认 config.json）。
 * - 使用 nlohmann::json 解析，填充 Config 结构体。
 * - 使用 .value() 方法处理可选字段，提供默认值。
 * - 异常处理：若文件不存在或格式错误，应抛出异常或返回默认配置。
 *
 * 扩展点：
 * - 第四阶段需支持命令行参数覆盖（如 --source），此处可预留接口。
 */

#include "config.h"
#include <fstream>
// struct Config{
//     std::string source_dir;                 // 源目录路径
//     std::string target_dir;                 // 目标目录路径
//     int sync_interval_secs = 5;             // 轮询间隔，单位秒，默认 5 秒
//     std::vector<std::string> exclude_patterns; // 排除规则，正则表达式列表

//     // 日志配置
//     std::string log_level = "INFO";         // 日志级别：DEBUG/INFO/ERROR
//     std::string log_file_path = "./logs/file_sync.log"; // 日志文件路径

//     // 重试配置
//     int max_retries = 3;                    // 最大重试次数，默认 3 次
//     int retry_delay_secs = 2;               // 重试间隔，单位秒，默认 2 秒
// };
Config load_config(const std::string& path) 
{
    std::ifstream f(path);
    if (!f.is_open()) {
        throw std::runtime_error("Failed to open config file: " + path);
    }
    try{
    Config config;
    json j;
    f>>j;
    config.source_dir=j["source_dir"];
    config.target_dir=j["target_dir"];
    config.sync_interval_secs=j["sync_interval_secs"];
    config.exclude_patterns=j["exclude_patterns"].get<std::vector<std::string>>();
    config.log_level=j["log"]["level"];
    config.log_file_path=j["log"]["file_path"];
    config.max_retries=j["retry"]["max_attempts"];
    return config;

    }catch(const json::exception &e){
        throw std::runtime_error("JSON丢失"+std::string(e.what()));
    }
    

}