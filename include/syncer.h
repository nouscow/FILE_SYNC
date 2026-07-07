/**
 * @file syncer.h
 * @brief 文件同步执行器：将源文件复制到目标目录，支持覆盖和删除。
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

    // 将源文件复制到目标目录，自动创建父目录，覆盖已有文件
    bool sync_file(const FileInfo& file);

    // 删除目标目录中的指定文件
    bool delete_file(const std::string& relative_path);

    // TODO: 实现重试机制（指数退避）
    //       在 sync_file() 失败时按 retry 配置重试，
    //       可抽取 retry_with_backoff() 私有方法

    // TODO: Linux 下 sendfile 零拷贝优化
    //       大文件传输时替换 copy_file 为 sendfile 系统调用
};


#endif // SYNCER_H
