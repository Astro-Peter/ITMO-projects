cmake_minimum_required(VERSION 3.12)
project(labwork_8_Astro_Peter)

set(CMAKE_CXX_STANDARD 20)

link_directories(lib)

add_subdirectory(test)
add_subdirectory(lib)
add_subdirectory(rubbish_bin)

enable_testing()
