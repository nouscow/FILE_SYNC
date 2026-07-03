/**
 * @file scanner.h
 * @brief 目录扫描器声明
 *
 * 功能：
 * - 递归遍历源目录，收集所有文件的相对路径、最后修改时间、文件大小。
 * - 提供静态方法 diff()，对比源文件列表和目标文件列表，返回需要同步的文件列表。
 *
 * 数据结构：
 * - FileInfo：存储单个文件的相对路径、修改时间、大小。
 * - Scanner：持有源目录路径，提供 scan() 方法。
 *
 * 注意事项：
 * - 使用 std::filesystem::recursive_directory_iterator 遍历。
 * - 相对路径用于后续在目标目录中重建相同结构。
 * - diff() 的比较依据：文件不存在或修改时间/大小不同则视为需要同步。
 */

#ifndef SCANNER_H
#define SCANNER_H
#include<string>
#include<filesystem>
#include<vector>
#include<map>
#include<set>
namespace fs = std::filesystem;
struct FileInfo {
    fs::path path;//相对路径
	fs::file_time_type last_change_time;//最后修改时间
	int file_size;//文件大小
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
	bool operator<(const FileInfo& other) const {
		return file_size < other.file_size; // 按文件名升序
		
	}
};


class Scanner{
   private:
	  
	 
	std::vector<FileInfo>list_files(const fs::path& dir);
   public:
	  std::set<FileInfo> diff(std::vector<FileInfo>sour, std::vector<FileInfo>tar);
      std::set<FileInfo> scan(const fs::path& source, const fs::path& targe);

};

#endif // SCANNER_H
