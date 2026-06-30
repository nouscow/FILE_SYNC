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

#include "scanner.h"
