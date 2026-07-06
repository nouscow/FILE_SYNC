cmake_minimum_required(VERSION 3.16)

# ---- 项目信息 ----
project(file_sync VERSION 1.0 LANGUAGES CXX)

# ---- C++ 标准 ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- 输出目录 ----
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---- 源文件 ----
set(SOURCES
    src/main.cpp
    src/config.cpp
    src/logger.cpp
    src/scanner.cpp
    src/syncer.cpp
    src/monitor.cpp  
    src/inotify_monitor.cpp
    src/polling_monitor.cpp
)

# ---- 头文件路径 ----
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/thirdparty)

# ---- 可执行目标 ----
add_executable(${PROJECT_NAME} ${SOURCES})

# ---- 头文件目录（私有，不泄露给其他目标） ----
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_INCLUDE_DIR}
    ${THIRDPARTY_DIR}
)

# ---- 平台宏定义 ----
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
    # 在 Linux 下链接 pthread（如果将来使用 std::thread 需要，但 C++17 通常不需要显式链接）
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif()

# ---- 配置文件复制 ----
# 将 config.json 复制到可执行文件所在的输出目录
configure_file(
    ${CMAKE_SOURCE_DIR}/config.json
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config.json
    COPYONLY
)

# ---- 创建日志目录（可选，运行时自动创建更好） ----
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/logs)

# ---- 编译选项 ----
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# ---- 安装（可选） ----
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${CMAKE_SOURCE_DIR}/config.json DESTINATION bin)