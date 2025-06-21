cmake_minimum_required(VERSION 3.10)
project(ChatClient)

set(CMAKE_CXX_STANDARD 17)
include_directories(../common)
add_executable(client client.cpp)
find_package(Threads REQUIRED)
target_link_libraries(client PRIVATE Threads::Threads)
