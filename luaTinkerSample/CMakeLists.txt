include_directories(../lua515)
include_directories(../lua_tinker)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/luaTinkerSample)

add_executable(luaTinkerSample sample.cpp)
target_link_libraries(luaTinkerSample lua luatinker)
