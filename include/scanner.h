/**
 * @file scanner.h
 * @brief 目录扫描器：递归遍历收集文件元数据，对比源/目标差异。
 */

#ifndef SCANNER_H
#define SCANNER_H
#include<string>
#include<filesystem>
#include<vector>
#include<map>
#include<set>
namespace fs = std::filesystem;

// 单个文件的元数据
struct FileInfo {
    fs::path path;                          // 相对路径
    fs::file_time_type last_change_time;    // 最后修改时间
    int file_size;                          // 文件大小

    // 三字段全部相同视为相等
    bool operator==(const FileInfo &other) {
        if (this->path == other.path&&this->file_size == other.file_size && this->last_change_time == other.last_change_time) {
            return true;
        }
        else {
            return false;
        }
    }
    bool operator!=(const FileInfo& other) {
        if (this->path == other.path && this->file_size == other.file_size && this->last_change_time == other.last_change_time) {
            return false;
        }
        else {
            return true;
        }
    }

    // 按相对路径升序排序，用于 std::set / set_symmetric_difference 操作
    bool operator<(const FileInfo& other) const {
        return this->path < other.path;
    }
};


class Scanner{
   private:
      // 递归遍历目录，收集所有文件的 FileInfo
      std::vector<FileInfo>list_files(const fs::path& dir);

      // TODO: 实现 exclude_patterns 过滤
      //       在 list_files() 中跳过匹配 exclude_patterns 的文件
      //       需要传入 exclude_patterns 参数或存储为成员变量

   public:
      // 对两组 FileInfo 做对称差集，返回需要处理的文件集合
      std::set<FileInfo> diff(std::vector<FileInfo>sour, std::vector<FileInfo>tar);

      // 扫描源目录和目标目录，返回差异文件集合
      std::set<FileInfo> scan(const fs::path& source, const fs::path& targe);
};

#endif // SCANNER_H
