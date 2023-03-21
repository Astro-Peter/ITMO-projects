add_executable(lab8 main.cpp)

target_link_libraries(lab8 circbuffer)
target_include_directories(lab8 PRIVATE ${PROJECT_SOURCE_DIR})