cmake_minimum_required(VERSION 3.16)

# ---- 项目信息 ----
project(file_sync VERSION 1.0 LANGUAGES CXX)

# ---- C++ 标准 ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- 输出目录 ----
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---- 公共源文件（所有平台共用） ----
set(COMMON_SOURCES
    src/main.cpp
    src/config.cpp
    src/logger.cpp
    src/scanner.cpp
    src/syncer.cpp
    src/monitor.cpp
)

# ---- 平台相关源文件 ----
if(WIN32)
    # Windows 监控实现
    list(APPEND COMMON_SOURCES src/polling_monitor.cpp)
elseif(APPLE)
    # macOS 监控实现
    list(APPEND COMMON_SOURCES src/polling_monitor.cpp)   
else()
    # Linux 使用 inotify
    list(APPEND COMMON_SOURCES src/inotify_monitor.cpp)
endif()

# ---- 头文件路径 ----
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/thirdparty)

# ---- 可执行目标 ----
add_executable(${PROJECT_NAME} ${COMMON_SOURCES})

# ---- 头文件目录 ----
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_INCLUDE_DIR}
    ${THIRDPARTY_DIR}
)

# ---- 线程库（所有平台都需要，Windows 下也会自动处理） ----
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# ---- 平台宏定义 ----
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
endif()

# ---- 配置文件复制（使用生成器表达式，兼容多配置生成器） ----
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/config.json
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/config.json
    COMMENT "Copying config.json to output directory"
)

# ---- 创建日志目录（在构建目录下，同时也在可执行文件目录下） ----
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/logs)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/logs
    COMMENT "Creating logs directory next to executable"
)

# ---- 编译选项 ----
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
)

# ---- 安装 ----
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${CMAKE_SOURCE_DIR}/config.json DESTINATION bin)