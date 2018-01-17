include_directories(./../lua515)
include_directories(./../lua_tinker)

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/sample)
add_executable(sample  sample.cpp)
target_link_libraries(sample lua luatinker)
