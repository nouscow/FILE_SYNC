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
class Scanner{

};


#endif // SCANNER_H
// ... 结构体和类声明 ...