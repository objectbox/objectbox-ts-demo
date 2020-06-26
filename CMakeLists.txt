cmake_minimum_required(VERSION 3.16)
project(objectbox_ts_demo)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)
include_directories(include)
include_directories(src/util)

link_directories(lib)

add_executable(${PROJECT_NAME} src/main.cpp src/util/StopWatch.cpp)
target_link_libraries(${PROJECT_NAME} objectbox)
