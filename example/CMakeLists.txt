cmake_minimum_required(VERSION 3.26)
project(simple_db)

set(CMAKE_CXX_STANDARD 23)

add_executable(example_simple_db main.cpp)

target_include_directories(example_simple_db PRIVATE simple_db)
target_link_libraries(example_simple_db simple_db)
