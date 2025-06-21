cmake_minimum_required(VERSION 3.10)
project(ChatServer)

set(CMAKE_CXX_STANDARD 17)

include_directories(../common)

add_executable(server server.cpp)

# 链接线程库
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)

# 自动复制 server.ini 到构建目录
add_custom_command(TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/server.ini"
            "$<TARGET_FILE_DIR:server>/server.ini"
    COMMENT "Copying server.ini to output directory"
)
