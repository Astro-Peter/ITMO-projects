cmake_minimum_required(VERSION 3.23)
project(labwork_9_Astro_Peter)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(alloc_lib)

add_executable(main main.cpp)

target_link_libraries(main alloc)