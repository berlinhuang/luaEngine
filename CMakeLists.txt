cmake_minimum_required(VERSION 2.6)

project(luaEngine)

add_subdirectory(lua515)
add_subdirectory(chat)
add_compile_options(-std=c++11)
add_subdirectory(lua_tinker)
add_subdirectory(sample)



