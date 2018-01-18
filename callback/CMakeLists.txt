#include_directories(/home/berlin/code/github/build/release-install/include)
#
#link_directories(/home/berlin/code/github/build/release-install/lib)



include_directories(./../lua515)
include_directories(./../lua_tinker)


set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/callback)

add_executable(callback callback.cpp)
target_link_libraries(callback lua)
