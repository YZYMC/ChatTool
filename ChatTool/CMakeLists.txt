cmake_minimum_required(VERSION 3.10)
project(ChatClient)

set(CMAKE_CXX_STANDARD 17)

include_directories(../common)

add_executable(client client.cpp)

# 链接线程库
find_package(Threads REQUIRED)
target_link_libraries(client PRIVATE Threads::Threads)

# 自动复制 client.ini 到构建目录
add_custom_command(TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/client.ini"
            "$<TARGET_FILE_DIR:client>/client.ini"
    COMMENT "Copying client.ini to output directory"
)
