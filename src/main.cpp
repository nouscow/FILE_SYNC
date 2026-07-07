/**
 * @file main.cpp
 * @brief 程序入口：加载配置，初始化日志，启动监控循环。
 *
 * 流程：信号注册 → 加载配置 → 初始化日志 → 创建模块 → 启动监控 → 等待退出信号
 */

#include <iostream>
#include <csignal>
#include <atomic>
#include <nlohmann/json.hpp>
#include <thread>
#include <chrono>
#include <memory>
#include <filesystem>
#include "config.h"
#include "logger.h"
#include "inotify_monitor.h"
#include "polling_monitor.h"
#include "scanner.h"
#include "syncer.h"
#include "monitor.h"

std::atomic<bool> quit(false);

// 信号处理函数：设置退出标志，记录日志
void signal_handler(int) {
    quit = true;
    Logger::get().info("收到退出信号，正在停止...");
}

// 程序入口：组装所有模块并启动事件循环
int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // TODO: CLI 命令行参数支持
    //       解析 --source、--target、--interval 等参数，覆盖 config.json 中的值
    Config cfg = load_config("config.json");
    std::cout << "souce_dir: " << cfg.source_dir << "\ntarget_dir: " << cfg.target_dir << std::endl;

    // 初始化日志单例
    Logger::init(cfg.log_file_path, cfg.log_max_size_mb * 1024L * 1024L, cfg.log_backup_count);
    Logger::get().info("文件同步工具启动");

    // 确保源目录和目标目录存在
    std::filesystem::create_directories(cfg.source_dir);
    std::filesystem::create_directories(cfg.target_dir);

    // 初始化扫描器和同步器
    Scanner scanner=Scanner();
    Syncer syncer(cfg.source_dir, cfg.target_dir);

    // 核心同步回调：扫描 → 差异 → 同步/删除
    auto sync_callback = [&]() {
        Logger::get().info("开始扫描...");
        auto changes = scanner.scan(cfg.source_dir,cfg.target_dir);

        if (changes.empty()) {
            Logger::get().info("没有变化");
            return;
        }

        for (const auto& file : changes) {
            std::string rel_path = file.path.string();
            std::filesystem::path src_path = std::filesystem::path(cfg.source_dir) / file.path;
            if (std::filesystem::exists(src_path)) {
                // 源文件存在 → 同步（新增或修改）
                if (syncer.sync_file(file)) {
                    Logger::get().info("已同步: " + rel_path);
                }
                else {
                    Logger::get().error("同步失败: " + rel_path);
                }
            }
            else {
                // 源文件不存在 → 删除目标端对应文件
                if (syncer.delete_file(rel_path)) {
                    Logger::get().info("已删除: " + rel_path);
                }
                else {
                    Logger::get().error("删除失败: " + rel_path);
                }
            }
        }
        };

    // 根据平台选择监控策略
    std::unique_ptr<Monitor>monitor;
    #ifdef __linux__
    monitor=std::make_unique<inotify_monitor>(cfg.source_dir, sync_callback);
    #else
    monitor=std::make_unique<polling_monitor>(cfg.sync_interval_secs, sync_callback);
    #endif

    monitor->start();
    Logger::get().info("监控已启动，按 Ctrl+C 停止");
    std::cout << "it is stop by Ctrl+C" << std::endl;

    // 主线程阻塞等待退出信号
    while (!quit) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    monitor->stop();
    Logger::get().info("程序正常退出");

    return 0;
}
