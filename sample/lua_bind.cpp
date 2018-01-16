//
// Created by root on 18-1-16.
//

#include "lua_bind.h"

int my_getpoint(lua_State* L)
{
    int top = lua_gettop(L);
    lua_tinker::user* pUser = (lua_tinker::user*) lua_touserdata(L, -1);
    if (!pUser)
    {
        lua_pushnil(L);
        return 1;
    }
    lua_pushlightuserdata(L, pUser->m_p);
    return 1;
}
//
//
//bool customLuaLoader(lua_State* L)
//{
//	int nTop = lua_gettop(L);
//	// 获得require函数的env
//
//	const char* path = NULL;
//	lua_getfield(L, LUA_GLOBALSINDEX, "require");
//	lua_getfenv(L, -1);
//	int env = lua_gettop(L);
//	if (!lua_istable(L, -1))
//		luaL_error(L, LUA_QL("require env") " must be a table");
//
//	lua_getfield(L, env, "loaders");
//	if (!lua_istable(L, -1))
//		luaL_error(L, LUA_QL("package.loaders") " must be a table");
//
//	lua_pushcfunction(L, my_loader_Lua);
//	int nNewLuaLoader = lua_gettop(L);
//	//获取原lua_loader的函数环境
//	lua_rawgeti(L, -2, 2);
//	lua_getfenv(L, -1);
//	lua_setfenv(L, nNewLuaLoader);
//
//	lua_settop(L, nNewLuaLoader);
//
//	lua_rawseti(L, -2, 2);
//
//
//	lua_settop(L, nTop);	// 还原栈，避免出问题
//	return true;
//}
