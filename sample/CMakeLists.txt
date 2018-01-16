include_directories(./../lua515)
include_directories(./../lua_tinker)
set(sample_SRCS
        lua_bind.cpp
        sample.cpp
        )
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/sample)
add_executable(sample ${sample_SRCS})
target_link_libraries(sample lua luatinker)
