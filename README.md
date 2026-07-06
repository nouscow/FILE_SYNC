# FILE_SYNC


**FILE_SYNC** 是一个跨平台的文件目录实时同步工具，使用 C++17 编写。它能够高效地监控源目录中的文件变更（创建、修改、删除），并自动同步到目标目录。项目采用模块化设计，通过平台宏 (`PLATFORM_WINDOWS` / `PLATFORM_LINUX`) 在单个 `monitor.cpp` 中实现了 Windows 轮询与 Linux inotify 两种监控策略，为后续扩展提供了清晰的架构基础。

---

## ✨ 特性

- **跨平台**：Windows 下使用轮询（Polling），Linux 下使用 `inotify` 实时监控，一套代码双平台运行。
- **实时同步**：Linux 版基于 `inotify` 事件驱动，文件变更后亚秒级触发同步。
- **配置驱动**：通过 `config.json` 灵活配置源目录、目标目录、同步间隔等参数。
- **日志系统**：线程安全、按天滚动的日志记录，方便追踪同步历史和错误。
- **模块化设计**：`Monitor` 抽象基类 + 平台特化实现，易于扩展和维护。
- **优雅退出**（即将完成）：采用 `select` + self-pipe 模式，避免粗暴关闭文件描述符导致的竞态条件。

---

## 🚀 快速开始

### 环境要求

- 编译器：支持 C++17 的 GCC 8+, Clang 10+, MSVC 2019+
- 构建系统：CMake >= 3.16
- 第三方库：`nlohmann/json`（已包含在 `thirdparty/` 目录下）

### 构建与运行

```bash
git clone https://github.com/nouscow/FILE_SYNC.git
cd FILE_SYNC
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# 编辑配置文件（可选）
vim ../config.json

<<<<<<< HEAD


=======
>>>>>>> 2372363cac9d238f0d3a42f1a0f3fe64d05afb27
# 运行
./bin/file_sync
```

首次运行会自动创建 `logs/` 目录，并生成当日日志文件。

---

## 📁 项目结构

```
FILE_SYNC/
├── include/
│   ├── config.h          # 配置结构体与加载函数
│   ├── logger.h          # 日志类（线程安全，按天滚动）
│   ├── scanner.h         # 目录扫描与文件差异比对
│   ├── syncer.h          # 文件同步执行器
│   ├── polling_monitor.h          # 轮询目录监控
│   ├── inotify_monitor.h          # Linux 实时监控
│   └── monitor.h         # 目录监控抽象基类
├── src/
│   ├── main.cpp          # 程序入口，模块组装与事件循环
│   ├── config.cpp
│   ├── logger.cpp
│   ├── scanner.cpp
│   ├── syncer.cpp
│   ├── polling_monitor.cpp
│   ├── inotify_monitor.cpp
│   └── monitor.cpp  
├── thirdparty/
│   └── nlohmann/
│       └── json.hpp      # JSON 解析单头文件库
├── config.json           # 默认配置文件
├── CMakeLists.txt
└── README.md
```

---

## 🧠 技术架构

### 跨平台策略：单文件 + 条件编译

所有平台相关的监控逻辑均集中在 `monitor.cpp` 中，通过 CMake 定义的宏 `PLATFORM_WINDOWS` 或 `PLATFORM_LINUX` 进行条件编译。这样既保持了项目结构的简洁，又便于后续独立拆分。

```cpp
// monitor.cpp 中的核心选择
#if defined(PLATFORM_LINUX)
    // Linux inotify 实现
#elif defined(PLATFORM_WINDOWS)
    // Windows 轮询实现
#endif
```

### Linux 实时监控（inotify）

- **事件驱动**：使用 `inotify` 监听 `IN_CREATE | IN_MODIFY | IN_CLOSE_WRITE | IN_MOVED_TO` 事件，避免轮询带来的 CPU 空转。
- **工作线程**：`Monitor` 在独立线程中运行 `loop()` 函数，阻塞在 `read()` 上等待事件。
- **事件处理**：读取 `inotify_event` 结构体，提取文件名并调用回调函数触发同步。

### Windows 轮询监控

- **定时扫描**：每隔 `interval_secs` 秒遍历源目录，通过比较文件最后修改时间和大小判断是否需要同步。
- **简单可靠**：不依赖 Windows 特有 API，适合跨平台原型开发。

### 配置与日志

- **配置**：使用 `nlohmann/json` 解析 `config.json`，支持 `source_dir`、`target_dir`、`interval_seconds`、`verbose` 等字段。
- **日志**：`Logger` 类使用 `std::mutex` 保证线程安全，按天生成日志文件（`sync_YYYY-MM-DD.log`），支持 INFO/ERROR 级别。

---

## 📊 项目进度

| 阶段 | 功能 | 状态 |
|------|------|------|
| **Phase 1** | 跨平台项目骨架，Windows 轮询同步 | ✅ 完成 |
| **Phase 2** | Linux inotify 实时监控（基础版） | ✅ 完成 |
| **Phase 2.1** | `select` + self-pipe 优雅退出 | 🔜 进行中 |
| **Phase 3** | `sendfile` 零拷贝优化 (Linux) | 📅 规划中 |
| **Phase 4** | CLI 命令行参数支持 | 📅 规划中 |

---

## 🤝 贡献

欢迎任何形式的贡献！如果你有好的想法或发现了 Bug，请提出 Issue 或 Pull Request。

---

## 📄 许可

本项目采用 MIT 许可证。详情请参见 LICENSE 文件。