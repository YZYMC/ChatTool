cmake_minimum_required(VERSION 3.10)
project(ChatServer)

set(CMAKE_CXX_STANDARD 17)
include_directories(../common)
add_executable(server server.cpp)
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)
