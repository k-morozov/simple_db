cmake_minimum_required(VERSION 3.26)
project(simple_db)

set(CMAKE_CXX_STANDARD 23)
#SET(CMAKE_CXX_FLAGS "-std=c++23")
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_STANDARD 23)
#add_compile_options(-std=c++2c)
#add_compile_options(-stdlib=libc++)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -stdlib=libc++")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

add_subdirectory(src)
add_subdirectory(example)
add_subdirectory(contrib)
add_subdirectory(tests)