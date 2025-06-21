cmake_minimum_required(VERSION 3.10)
project(ChatTool)

set(CMAKE_CXX_STANDARD 17)

# 客户端目标
add_executable(client client.cpp ${SOURCES_COMMON})

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
