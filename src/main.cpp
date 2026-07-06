#include<iostream>/**
 * @file main.cpp
 * @brief 程序入口，组装所有模块并启动事件循环
 *
 * 流程：
 * 1. 注册信号处理函数（SIGINT/SIGTERM），用于优雅退出。
 * 2. 加载配置文件（config.json），打印基本信息。
 * 3. 初始化 Logger 实例。
 * 4. 确保源目录和目标目录存在。
 * 5. 创建 Scanner 和 Syncer 实例。
 * 6. 定义同步回调 lambda，内部调用 scanner.scan()、Scanner::diff()、syncer.sync_file()。
 * 7. 创建 Monitor 实例并启动。
 * 8. 主线程等待退出信号（while(!quit) sleep）。
 * 9. 收到信号后停止 Monitor，记录日志，退出。
 *
 * 注意事项：
 * - 全局 Logger 指针用于信号处理函数中记录日志，需保证线程安全。
 * - 使用 std::atomic<bool> quit 作为退出标志。
 * - 回调中应检查 quit 标志，避免在退出过程中执行不必要的同步。
 */
/**
 * ============================================================
 *  项目：跨平台文件同步工具 (file_sync)
 *  版本：v1.0
 *  作者：nousCow
 *  简述：实时监控源目录，将新增/修改的文件自动同步到目标目录。
 * ============================================================
 *
 * 结构体说明：
 *   Config   - 配置信息容器，从 config.json 加载，包含源目录、目标目录、
 *              轮询间隔、排除规则、日志配置、重试参数等。
 *   FileInfo - 单个文件的元数据，包括相对路径、最后修改时间、文件大小。
 *
 * 类说明：
 *   Logger   - 线程安全的日志记录器，支持 DEBUG/INFO/ERROR 三级，
 *              输出到文件并按天滚动。
 *   Scanner  - 目录扫描器，递归遍历源目录，返回 FileInfo 列表；
 *              提供静态方法 diff() 对比源/目标差异。
 *   Syncer   - 文件同步执行器，根据 FileInfo 将源文件复制到目标目录，
 *              自动创建父目录，支持覆盖和删除操作。
 *   Monitor  - 目录监控器（轮询模式），每隔固定秒数触发回调函数，
 *              用于驱动扫描→差异→同步流程。
 */

#include <iostream>
#include <csignal>
#include <atomic>
#include <nlohmann/json.hpp>
#include<thread>
#include<chrono>
#include"config.h"
#include"logger.h"
#include<filesystem>
#include"inotify_monitor.h"
#include"polling_monitor.h"
#include "scanner.h"
#include "syncer.h"
#include "monitor.h"
#include<memory>

std::atomic<bool> quit(false);
Logger* global_logger = nullptr;

void signal_handler(int) {
    quit = true;
    if (global_logger) {
        global_logger->info("收到退出信号，正在停止...");
    }
}

int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // 加载配置
    Config cfg = load_config("config.json");
    std::cout << "源目录: " << cfg.source_dir << "\n目标目录: " << cfg.target_dir << std::endl;

    // 初始化日志
    Logger logger(cfg.log_file_path);
    global_logger = &logger;
    logger.info("文件同步工具启动");

    // 确保目录存在
    std::filesystem::create_directories(cfg.source_dir);
    std::filesystem::create_directories(cfg.target_dir);
  
    //// 初始化模块
    Scanner scanner=Scanner();
    Syncer syncer(cfg.source_dir, cfg.target_dir);

    //// 定义同步回调（核心逻辑）
    auto sync_callback = [&]() {
        logger.info("开始扫描...");
        auto changes = scanner.scan(cfg.source_dir,cfg.target_dir);

        if (changes.empty()) {
            logger.info("没有变化");
            return;
        }

        for (const auto& file : changes) {
            std::string rel_path = file.path.string();

            // 检查源文件是否存在（可能存在但被删除的情况）
            std::filesystem::path src_path = std::filesystem::path(cfg.source_dir) / file.path;
            if (std::filesystem::exists(src_path)) {
                if (syncer.sync_file(file)) {
                    logger.info("已同步: " + rel_path);
                }
                else {
                    logger.error("同步失败: " + rel_path);
                }
            }
            else {
                if (syncer.delete_file(rel_path)) {
                    logger.info("已删除: " + rel_path);
                }
                else {
                    logger.error("删除失败: " + rel_path);
                }
            }
        }
        };

    // 启动监控
     std::unique_ptr<Monitor>monitor;
    #ifdef __linux__
    monitor=std::make_unique<inotify_monitor>(cfg.source_dir, sync_callback);

    #else
      monitor=std::make_unique<polling_monitor>(cfg.sync_interval_secs, sync_callback);
    #endif
   
    monitor->start();
    logger.info("监控已启动，按 Ctrl+C 停止");
    std::cout << "003" << std::endl;
    // 主线程等待退出信号
    while (!quit) {

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    monitor->stop();
    logger.info("程序正常退出");
    return 0;
}