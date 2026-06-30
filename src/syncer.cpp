/**
 * @file syncer.cpp
 * @brief 文件同步执行器的实现
 *
 * 实现要点：
 * - sync_file：拼接源路径和目标路径，创建父目录，复制文件。
 * - 使用 copy_options::overwrite_existing 覆盖已存在的文件。
 * - 错误处理：使用 std::error_code 捕获异常，避免抛出。
 * - 第三阶段可替换为 sendfile 系统调用（Linux 下）以提升性能。
 * - 第三阶段加入重试机制（指数退避）。
 *
 * 扩展：
 * - 增量同步：记录上次同步位置，使用 lseek + read 跳过已同步部分。
 */

#include "syncer.h"
// ... 实现 ...