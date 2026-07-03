/**
 * @file scanner.cpp
 * @brief 目录扫描器的实现
 *
 * 实现要点：
 * - scan() 调用私有方法 list_files() 递归遍历。
 * - list_files() 使用 recursive_directory_iterator，跳过目录自身。
 * - 使用 fs::relative() 获取相对于源目录的路径。
 * - diff() 使用 unordered_map 加速查找，时间复杂度 O(n+m)。
 * - 注意处理符号链接和权限错误（可忽略或记录日志）。
 *
 * 扩展：
 * - 第三阶段可加入排除规则过滤（在 scan() 中跳过匹配的文件）。
 */
 // * - scan() 调用私有方法 list_files() 递归遍历。
 //  * - list_files() 使用 recursive_directory_iterator，跳过目录自身。
 //  * - 使用 fs::relative() 获取相对于源目录的路径。
 //  * - diff() 使用 unordered_map 加速查找，时间复杂度 O(n+m)。
#include "scanner.h"
#include<filesystem>
#include<iostream>
#include<unordered_map>
#include<map>
#include<set>
#include<algorithm>
std::set<FileInfo>Scanner::diff(std::vector<FileInfo>sour, std::vector<FileInfo>tar) {
	std::set<FileInfo>res;
	std::set<FileInfo>source, target;
	for (auto it : sour) {
		source.insert(it);
	}
	for (auto&it : tar) {
		target.insert(it);
	}
	std::set_symmetric_difference(
	  source.begin(),source.end(),
	 target.begin(),target.end(),
	 std::inserter(res,res.begin())
	);
	
	return res;

}
std::vector<FileInfo>Scanner::list_files(const fs::path& dir) {
	std::vector<FileInfo>file;
	if (!fs::exists(dir) || !fs::is_directory(dir)) {
		return file;
	}
	try {
		for (const auto& entry : fs::recursive_directory_iterator(dir)) {
			FileInfo fi;
			fi.file_size = fs::file_size(entry.path());
			fi.path =fs::relative(entry.path(),dir);
			fi.last_change_time =fs::last_write_time(entry.path()) ;
			file.push_back(fi);
		}
	}
	catch (fs::filesystem_error& e) {
		std::cerr << e.what() << e.path1() << e.path2() << e.code() << std::endl;
		exit(0);
	}
	return file;
}
std::set<FileInfo> Scanner::scan(const fs::path& source, const fs::path& targe){
	std::vector<FileInfo>sour, tar;
	sour = list_files(source);
	tar= list_files(targe);
	std::set<FileInfo>res=diff(sour,tar);
	return res;

}
