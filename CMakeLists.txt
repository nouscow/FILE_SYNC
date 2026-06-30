# CMakeLists.txt
#
# 功能：构建系统配置
#
# 内容：
# - 设置最低 CMake 版本 3.16
# - 项目名称 file_sync，C++17 标准
# - 添加头文件目录 include/ 和第三方库目录 thirdparty/
# - 列出所有源文件（src/main.cpp, config.cpp, logger.cpp, scanner.cpp, syncer.cpp, monitor.cpp）
# - 生成可执行文件 file_sync
# - 平台宏定义：PLATFORM_WINDOWS 或 PLATFORM_LINUX
#
# 注意事项：
# - 使用 FetchContent 或本地路径引入 nlohmann/json。
# - 第二阶段需添加 Linux 特定源文件（如 inotify_monitor.cpp）。
# - 第三阶段可链接 pthread（如果使用线程池）。
cmake_minimum_required(VERSION 3.16)
project(file_sync VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加本地 json.hpp 路径
include_directories(${CMAKE_SOURCE_DIR}/thirdparty)

set(SOURCES
    src/main.cpp
    src/config.cpp
    src/logger.cpp
    src/scanner.cpp
    src/syncer.cpp
    src/monitor.cpp
)

# 不再使用 FetchContent
# include(FetchContent)
# FetchContent_Declare(...)

add_executable(${PROJECT_NAME} ${SOURCES})
# 不再需要 target_link_libraries 链接 json

# 平台宏定义
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
endif()