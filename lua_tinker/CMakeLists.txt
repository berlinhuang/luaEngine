include_directories(./../lua515)


find_library(LUA_LIB lua ${PROJECT_SOURCE_DIR}/lua515)
IF(NOT LUA_LIB)
    message(FATAL_ERROR "lua not found")
ENDIF(NOT LUA_LIB)
message(STATUS "lua found in " ${LUA_LIB})



add_library(luatinker     lua_tinker.cpp)
link_libraries(luatinker ${LUA_LIB})

