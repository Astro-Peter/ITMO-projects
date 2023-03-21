add_library(mp3_frames Frame.cpp)

# target_include_directories(mp3_frames PUBLIC ${PROJECT_SOURCE_DIR})

target_include_directories(mp3_frames PUBLIC ${PROJECT_SOURCE_DIR}/common_methods)