/**
 * @file config.h
 * @brief 配置结构体与加载函数声明
 *
 * 功能：
 * - 定义 Config 结构体，包含源目录、目标目录、轮询间隔、排除规则、日志配置、重试配置。
 * - 声明 load_config() 函数，从 JSON 文件读取配置并返回 Config 实例。
 *
 * 注意事项：
 * - 使用 nlohmann/json 库解析 JSON，需要包含 <nlohmann/json.hpp>。
 * - 所有路径字符串使用正斜杠 '/' 以保证跨平台兼容。
 * - 排除规则使用正则表达式字符串列表，后续由 Syncer 过滤。
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
    int sync_interval_secs = 5;             // 轮询间隔，单位秒，默认 5 秒
    std::vector<std::string> exclude_patterns; // 排除规则，正则表达式列表

    // 日志配置
    std::string log_level = "INFO";         // 日志级别：DEBUG/INFO/ERROR
    std::string log_file_path = "./logs/file_sync.log"; // 日志文件路径
    int log_max_size_mb = 10;               // 单个日志文件最大大小(MB)，超过则滚动
    int log_backup_count = 3;               // 滚动备份文件数量

    // 重试配置
    int max_retries = 3;                    // 最大重试次数，默认 3 次
    int retry_delay_secs = 2;    // 重试间隔，单位秒，默认 2 秒
};
Config load_config(const std::string& path);
void ConfigPrintf(const Config&config);
#endif // CONFIG_H
/**
 * ================================================================
 *  nlohmann/json 使用速查手册（适用于本项目）
 *  版本：v3.11.x
 *  头文件：thirdparty/nlohmann/json.hpp
 *  命名空间：nlohmann::json （通常简写为 json）
 * ================================================================
 *
 * 1. 包含头文件
 *    #include <nlohmann/json.hpp>
 *    using json = nlohmann::json;
 *
 * 2. 从文件读取 JSON
 *    std::ifstream f("config.json");
 *    json j;
 *    f >> j;                 // 直接将流读入 json 对象
 *    或：j = json::parse(f); // 另一种方式，效果相同
 *
 *    注意：如果文件不存在或格式错误，会抛出异常（json::parse_error）。
 *         生产环境建议用 try-catch 包裹。
 *
 * 3. 访问基本类型的值
 *    std::string s = j["key"];               // 获取字符串
 *    int i = j["number"];                    // 获取整数
 *    double d = j["pi"];                     // 获取浮点数
 *    bool b = j["flag"];                     // 获取布尔值
 *
 * 4. 提供默认值的访问（推荐用于可选字段）
 *    std::string s = j.value("key", "default_value");
 *    int i = j.value("number", 42);
 *    bool b = j.value("flag", false);
 *
 *    优点：如果 key 不存在，不会抛出异常，直接返回默认值。
 *
 * 5. 访问嵌套对象
 *    auto& log = j["log"];                   // log 是一个 json 对象引用
 *    std::string level = log.value("level", "INFO");
 *    std::string path  = log.value("file_path", "./logs/default.log");
 *
 *    或者链式调用：
 *    std::string level = j["log"].value("level", "INFO");
 *
 * 6. 访问数组
 *    json arr = j["exclude_patterns"];       // 获取数组
 *    for (const auto& item : arr) {
 *        std::string pattern = item.get<std::string>();
 *    }
 *    或直接转为 vector：
 *    std::vector<std::string> patterns = j["exclude_patterns"]
 *                                        .get<std::vector<std::string>>();
 *    注意：如果字段不存在，get<>() 会抛异常，建议先用 contains() 检查。
 *
 * 7. 判断字段是否存在
 *    if (j.contains("optional_field")) {
 *        // 存在才访问
 *    }
 *
 * 8. 写入 JSON 到文件（本项目中未用到，但了解无妨）
 *    std::ofstream out("output.json");
 *    out << j.dump(4);   // dump(缩进空格数) 生成美化后的字符串
 *
 * 9. 常见错误处理
 *    try {
 *        json j = json::parse(f);
 *    } catch (json::parse_error& e) {
 *        std::cerr << "JSON parse error: " << e.what() << std::endl;
 *    } catch (json::type_error& e) {
 *        std::cerr << "Type error: " << e.what() << std::endl;
 *    }
 *
 * 10. 本项目典型用法（config.cpp 中）
 *     Config load_config(const std::string& path) {
 *         std::ifstream f(path);
 *         json j;
 *         f >> j;                          // 读文件
 *
 *         Config cfg;
 *         cfg.source_dir = j["source_dir"]; // 必填字段，直接访问
 *         cfg.sync_interval_secs = j.value("sync_interval_secs", 5); // 可选字段
 *
 *         // 数组转 vector
 *         if (j.contains("exclude_patterns"))
 *             cfg.exclude_patterns = j["exclude_patterns"]
 *                                   .get<std::vector<std::string>>();
 *
 *         // 嵌套对象
 *         auto& log = j["log"];
 *         cfg.log_level = log.value("level", "INFO");
 *         cfg.log_file_path = log.value("file_path", "./logs/file_sync.log");
 *
 *         return cfg;
 *     }
 *
 * ================================================================
 *  总结：你只需要掌握以下四个操作就能完成本项目：
 *  1. 从文件读入：f >> j
 *  2. 获取值：j["key"] 或 j.value("key", default)
 *  3. 嵌套访问：j["parent"]["child"]
 *  4. 数组转 vector：j["arr"].get<vector<string>>()
 * ================================================================
 */
