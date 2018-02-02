include_directories(/home/berlin/code/github/build/release-install/include)

link_directories(/home/berlin/code/github/build/release-install/lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/luaScript)

add_executable(Client Client.cc)
target_link_libraries(Client muduo_base muduo_net pthread)

#add_compile_options(-std=c++11)

include_directories(../lua515)
include_directories(../lua_tinker)


add_executable(LuaServer Main.cpp LuaStart.cpp LuaServer.cpp)
target_link_libraries(LuaServer lua luatinker)
target_link_libraries(LuaServer muduo_base muduo_net pthread)
