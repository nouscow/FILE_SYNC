/**
 * @file scanner.cpp
 * @brief 目录扫描器实现：list_files() 递归遍历，diff() 对称差集对比。
 */

#include "scanner.h"
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>

// 对称差集对比：将两组 FileInfo 放入 std::set，
// 用 set_symmetric_difference 找出只存在于一侧或两侧不同的文件
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

// 递归遍历目录，收集所有条目的 FileInfo（路径、大小、修改时间）
// TODO: 跳过目录条目 — 当前 is_directory() 的条目也会被加入，
//       导致子目录本身出现在 diff 结果中，应添加 if (entry.is_directory()) continue;
// TODO: 实现 exclude_patterns 过滤 — 跳过匹配 exclude_patterns 正则的文件
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

// 组合调用：分别扫描源和目标目录，返回差异文件集合
std::set<FileInfo> Scanner::scan(const fs::path& source, const fs::path& targe){
	std::vector<FileInfo>sour, tar;
	sour = list_files(source);
	tar= list_files(targe);
	std::set<FileInfo>res=diff(sour,tar);
	return res;

}
