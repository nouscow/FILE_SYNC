/**
 * @file config.cpp
 * @brief 从 JSON 文件加载配置，填充 Config 结构体。
 */

#include "config.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "logger.h"
using nlohmann::json;

// 将 Config 关键字段打印到 stdout（启动时确认配置是否正确）
void ConfigPrintf(const Config&config){
    try{
        std::cout<<"source_dir:"<<config.source_dir<<std::endl;
        std::cout<<"target_dir:"<<config.target_dir<<std::endl;
        std::cout<<"sync_interval_secs:"<<config.sync_interval_secs<<" s"<<std::endl;
        std::cout<<"log_level :"<<config.log_level<<std::endl;
        std::cout<<"log_file_path :"<<config.log_file_path<<std::endl;
    }catch(const json::exception &e){
        throw std::runtime_error(std::string("JSON loss") + e.what());
    }
}

// 从 JSON 文件加载配置：打开文件 → 解析 → 填充 Config 各字段
// 必填字段用 j["key"] 直接访问，可选字段用 j.value("key", default)
Config load_config(const std::string& path)
{
    std::ifstream f(path);
    if (!f.is_open()) {
        throw std::runtime_error(std::string("Failed to open config file: ") + path);
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
        config.log_max_size_mb=j["log"].value("max_size_mb", 10);
        config.log_backup_count=j["log"].value("backup_count", 3);
        config.max_retries=j["retry"]["max_attempts"];

        // TODO: CLI 参数覆盖
        //       在此处合并命令行传入的参数，优先级高于 config.json

        return config;

    }catch(const json::exception &e){
        throw std::runtime_error(std::string("JSON丢失") + std::string(e.what()));
    }
}
