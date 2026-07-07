/**
 * @file syncer.cpp
 * @brief 文件同步实现：创建目标目录结构，复制/删除文件。
 */

#include "syncer.h"
#include <filesystem>
#include <fstream>

// 构造：保存源目录和目标目录路径
Syncer::Syncer(const std::string& source_dir, const std::string& target_dir):target_dir(target_dir),source_dir(source_dir){
}

// 同步单个文件：拼接路径 → 创建父目录 → 复制文件（覆盖已有）
// TODO: 实现重试机制 — 失败时按 retry 配置进行指数退避重试
// TODO: Linux sendfile 零拷贝 — 大文件时替换 copy_file 为 sendfile 系统调用
bool Syncer::sync_file(const FileInfo& file){
    fs::path src = fs::path(source_dir) / file.path;
    fs::path dst = fs::path(target_dir) / file.path;

    std::error_code ec;
    fs::create_directories(dst.parent_path(), ec);
    if (ec) return false;

    fs::copy_file(src, dst, fs::copy_options::overwrite_existing, ec);
    return !ec;
}

// 删除目标目录中的指定文件
bool Syncer::delete_file(const std::string& relative_path){
    fs::path dst = fs::path(target_dir) / relative_path;
    std::error_code ec;
    return fs::remove(dst, ec);
}
