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
#include"config.h"
using namespace std;
int main() {
   Config config=load_config("config.json");
    return 0;
}