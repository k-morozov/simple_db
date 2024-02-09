cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(src)

add_library(actor STATIC 
    actor.h
    types.h
)

# target_include_directories(actor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(actor PROPERTIES LINKER_LANGUAGE CXX)