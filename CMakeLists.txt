cmake_minimum_required(VERSION 3.21)
project(SimpleThreadPool)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_executable(SimpleThreadPool main.cpp ThreadPool.cpp ThreadPool.h)
target_link_libraries(SimpleThreadPool PUBLIC Threads::Threads)

