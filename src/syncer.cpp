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
#include <filesystem>
#include <fstream>
   Syncer::Syncer(const std::string& source_dir, const std::string& target_dir):target_dir(target_dir),source_dir(source_dir){
    
   }
    bool Syncer::sync_file(const FileInfo& file){
        fs::path src = fs::path(source_dir) / file.path;
        fs::path dst = fs::path(target_dir) / file.path;

        // 创建目标目录的父目录
        std::error_code ec;
        fs::create_directories(dst.parent_path(), ec);
        if (ec) return false;

        // 复制文件（覆盖已存在的）
        fs::copy_file(src, dst, fs::copy_options::overwrite_existing, ec);
        return !ec;
      
    }
    bool Syncer::delete_file(const std::string& relative_path){
        fs::path dst = fs::path(target_dir) / relative_path;
        std::error_code ec;
        return fs::remove(dst, ec);
  
    }