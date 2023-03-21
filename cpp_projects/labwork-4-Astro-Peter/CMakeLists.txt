cmake_minimum_required(VERSION 3.23)
project(labwork_4_Astro_Peter)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(archive_methods)

link_libraries(archive )
add_executable(labwork_4_Astro_Peter main.cpp)