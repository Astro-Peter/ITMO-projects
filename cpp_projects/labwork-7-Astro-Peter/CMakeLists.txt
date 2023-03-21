cmake_minimum_required(VERSION 3.23)
project(lab7)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(frame_classes)

add_subdirectory(common_methods)

add_subdirectory(tags)

add_executable(main main.cpp)

target_link_libraries(main mp3_frames mp3_methods mp3_tags)
