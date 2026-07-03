/**
 * @file syncer.h
 * @brief 文件同步执行器声明
 *
 * 功能：
 * - 根据 FileInfo 将源文件复制到目标目录（保持相对路径结构）。
 * - 提供 delete_file() 方法删除目标目录中的文件（用于处理源文件被删除的情况）。
 *
 * 接口：
 * - Syncer(const std::string& source_dir, const std::string& target_dir)
 * - bool sync_file(const FileInfo& file)
 * - bool delete_file(const std::string& relative_path)
 *
 * 注意事项：
 * - 复制前自动创建父目录（std::filesystem::create_directories）。
 * - 使用 std::filesystem::copy_file 并指定 overwrite_existing。
 * - 返回 bool 表示操作是否成功。
 */

#ifndef SYNCER_H
#define SYNCER_H
#include<string>
#include"scanner.h"
class Syncer{
  private:
    const std::string source_dir;
    const std::string target_dir;
  public:
    Syncer(const std::string& source_dir, const std::string& target_dir);
    bool sync_file(const FileInfo& file);
    bool delete_file(const std::string& relative_path);
};


#endif // SYNCER_H
