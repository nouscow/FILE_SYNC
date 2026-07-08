# FILE_SYNC

跨平台文件目录同步工具，使用 C++17 编写。监控源目录中的文件变更（新增、修改、删除），自动同步到目标目录。Windows/macOS 下使用定时轮询，Linux 下使用 inotify 事件驱动。

## 功能

- **定时轮询同步**（Windows/macOS）：按配置的间隔扫描源目录，对比差异后同步变更文件。
- **inotify 事件同步**（Linux）：通过 `inotify` 监听目录事件，有变更时立即触发同步。
- **JSON 配置**：通过 `config.json` 指定源目录、目标目录、轮询间隔、日志路径等参数。
- **日志记录**：静态单例模式（`Logger::init()` 初始化，`Logger::get()` 获取引用），线程安全的文件日志，支持 DEBUG/INFO/ERROR 三级过滤，输出格式为 `[LEVEL][YYYY-MM-DD HH:MM:SS][消息]`，支持按文件大小滚动和备份。
- **信号处理**：响应 SIGINT/SIGTERM（Ctrl+C）优雅退出。
- **目录自动创建**：启动时自动创建配置中指定的源目录和目标目录；同步时自动在目标端创建缺失的子目录。

## 环境要求

- 编译器：支持 C++17 的 MSVC 2019+、GCC 8+、Clang 10+
- 构建系统：CMake >= 3.16
- 第三方库：nlohmann/json（已包含在 `thirdparty/` 中，无需额外安装）

## 构建与运行

### Windows（Visual Studio）

```bash
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config Debug
```

构建完成后，可执行文件位于 `build/bin/Debug/file_sync.exe`，config.json 和 logs 目录会由 post-build 步骤自动复制到 exe 同级目录。

在 VS Code 中打开项目文件夹后，按 F5 即可调试运行（已配置 `.vscode/launch.json`）。

### Linux / macOS

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
./bin/file_sync
```

程序启动后会在控制台打印源目录和目标目录路径，然后进入后台轮询/inotify 监听循环。按 Ctrl+C 停止。

## 项目结构

```
FILE_SYNC/
├── include/
│   ├── config.h              Config 结构体定义与 load_config() 声明
│   ├── logger.h              Logger 类（线程安全日志）
│   ├── scanner.h             FileInfo 结构体、Scanner 类（目录扫描与差异比对）
│   ├── syncer.h              Syncer 类（文件复制与删除）
│   ├── monitor.h             Monitor 抽象基类（定义 start/stop/loop 接口）
│   ├── polling_monitor.h     polling_monitor 子类（轮询实现）
│   └── inotify_monitor.h     inotify_monitor 子类（Linux inotify 实现）
├── src/
│   ├── main.cpp              程序入口：加载配置、组装模块、信号处理、事件循环
│   ├── config.cpp            load_config() 实现，解析 config.json
│   ├── logger.cpp            Logger 实现，互斥锁保护的日志写入
│   ├── scanner.cpp           list_files() 递归遍历、diff() 对称差集比对、scan() 组合调用
│   ├── syncer.cpp            sync_file() 文件复制、delete_file() 文件删除
│   ├── monitor.cpp           Monitor 基类构造函数实现
│   ├── polling_monitor.cpp   轮询监控：sleep + callback 循环
│   └── inotify_monitor.cpp   inotify 监控：poll() 超时轮询 + read() 读取事件（仅 Linux 编译）
├── thirdparty/
│   └── nlohmann/
│       └── json.hpp          nlohmann/json 单头文件库
├── config.json               默认配置文件
├── CMakeLists.txt            CMake 构建脚本
└── LICENSE                   MIT 许可证
```

## 架构说明

### 模块关系

```
main.cpp
  ├─ load_config()  ──→  Config 结构体
  ├─ Logger           ──→  logs/file_sync.log
  ├─ Scanner          ──→  scan(source, target) → set<FileInfo>
  ├─ Syncer           ──→  sync_file() / delete_file()
  └─ Monitor (抽象基类)
       ├─ polling_monitor   (Windows/macOS)
       └─ inotify_monitor   (Linux)
```

程序启动时，`main()` 加载配置、初始化 Logger、创建 Scanner 和 Syncer 实例，然后定义一个同步回调 lambda。该回调的核心流程是：`scanner.scan()` 扫描源和目标目录 → `Scanner::diff()` 用 `std::set_symmetric_difference` 计算差异集 → 遍历差异集，对每个文件调用 `syncer.sync_file()` 或 `syncer.delete_file()`。

Monitor 使用多态：`main()` 中通过 `#ifdef __linux__` 选择创建 `inotify_monitor` 或 `polling_monitor`，以 `std::unique_ptr<Monitor>` 持有。调用 `start()` 启动后台线程，`stop()` 等待线程退出。

### 差异检测机制

Scanner 的 `scan()` 方法分别对源目录和目标目录调用 `list_files()`（基于 `std::filesystem::recursive_directory_iterator`），收集所有文件的相对路径、大小和最后修改时间。然后 `diff()` 将两组 FileInfo 放入 `std::set`，通过 `std::set_symmetric_difference` 找出只存在于一侧或两侧不同的文件。FileInfo 的 `operator<` 按 `path` 排序以支持 set 操作。

### 文件同步

Syncer 的 `sync_file()` 根据 FileInfo 的相对路径拼接出源和目标的绝对路径，先用 `std::filesystem::create_directories` 创建目标端的父目录，再用 `std::filesystem::copy_file` 配合 `overwrite_existing` 选项复制文件。`delete_file()` 用 `std::filesystem::remove` 删除目标端对应文件。两者都使用 `std::error_code` 版本的 API 避免异常抛出，直接返回 bool 表示成功与否。

### 平台监控策略

- **polling_monitor**：后台线程中 `loop()` 循环执行 `sleep_for(interval_secs)` → 检查 `running_` 标志 → 调用 callback。简单可靠，不依赖平台特有 API。
- **inotify_monitor**：`start()` 中调用 `inotify_init()` 和 `inotify_add_watch()` 注册 `IN_ALL_EVENTS`。`loop()` 使用 `poll()` 以 500ms 超时轮询 inotify fd，收到 `POLLIN` 后 `read()` 读取事件并解析 `inotify_event` 结构体，对符合条件的文件事件触发 callback。`poll()` 超时机制确保 `stop()` 能在 500ms 内响应，避免 `read()` 永久阻塞。整个实现用 `#ifdef __linux__` 包裹，非 Linux 平台不参与编译。

## 配置说明

`config.json` 示例：

```json
{
    "source_dir": "./test_source",
    "target_dir": "./test_target",
    "sync_interval_secs": 5,
    "exclude_patterns": [".tmp$", ".swp$", "~$"],
    "log": {
        "level": "INFO",
        "file_path": "./logs/file_sync.log",
        "max_size_mb": 10,
        "backup_count": 3
    },
    "retry": {
        "max_attempts": 3,
        "base_delay_ms": 1000
    }
}
```

当前实际生效的字段：

| 字段 | 说明 | 默认值 |
|------|------|--------|
| `source_dir` | 源目录路径 | 无（必填） |
| `target_dir` | 目标目录路径 | 无（必填） |
| `sync_interval_secs` | 轮询间隔（秒），仅 polling_monitor 使用 | 5 |
| `log.level` | 日志级别（DEBUG/INFO/ERROR） | INFO |
| `log.file_path` | 日志文件路径 | ./logs/file_sync.log |
| `log.max_size_mb` | 单个日志文件最大大小（MB），超过则滚动到备份文件 | 10 |
| `log.backup_count` | 滚动备份文件数量，超出的最旧备份会被删除 | 3 |

以下字段在 Config 结构体中有定义、`load_config()` 会解析，但当前同步流程中尚未实际使用：

- `exclude_patterns`：排除规则（正则表达式列表），Scanner 遍历和 Syncer 同步时均未过滤。
- `retry.max_attempts` / `retry.base_delay_ms`：重试参数，Syncer 当前未实现重试逻辑。

## 已知限制

- **排除规则未生效**：`exclude_patterns` 被解析到 Config 中但未在 Scanner 或 Syncer 中使用。
- **无增量同步**：每次同步都是全量对比 + 整文件复制，不支持断点续传或差量传输。
- **inotify 仅监听单层目录**：当前 `inotify_add_watch()` 只注册了顶层目录，子目录内的变更不会触发事件，需递归添加 watch 或改用 fanotify。

## 待做事项

- 实现 exclude_patterns 过滤（在 Scanner::list_files 中跳过匹配文件）
- 实现 Syncer 重试机制（指数退避）
- Linux 下 sendfile 零拷贝优化大文件传输
- CLI 命令行参数支持（`--source`、`--target`、`--interval` 等）

## 许可

MIT License，详见 LICENSE 文件。
