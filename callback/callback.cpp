//
// Created by root on 18-1-18.
//

#include "callback.h"
#include <functional>
#include <iostream>
using namespace std;

using ConnectEvent = std::function<void()>;
static int lua_callback = LUA_REFNIL;


static int setnotify(lua_State *L)
{
    lua_callback = luaL_ref(L, LUA_REGISTRYINDEX);// 对栈顶的值v生成返回一个引用r，即将[引用r， 栈顶值v]存到LUA_REGISTRYINDEX表中
    return 0;
}

static int testnotify(lua_State *L)
{
    // 将一个引用值入栈
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_callback);
    lua_pushnumber(L, 2);//params
    lua_pushnumber(L, 2);
    lua_pushnumber(L, 2);
    int status = lua_pcall(L, 3, 0, 1);//L当前栈，参数个数，返回值个数，发生错误处理时代码返回
}

static int testenv(lua_State *L)
{
    lua_getglobal(L, "defcallback");
    lua_call(L, 0, 0);
}




static const luaL_Reg cblib[] = {
        {"setnotify", setnotify},
        {"testnotify", testnotify},
        {"testenv", testenv},
        {NULL, NULL}
};

int luaopen_cb(lua_State *L)
{
    luaL_register(L, "cb", cblib);
    return 1;
}

int main()
{
    lua_State* L = luaL_newstate();
    luaopen_base(L);
    luaL_openlibs(L);
    luaopen_cb(L);
    ConnectEvent onConnected= std::bind(testnotify, L);

    int ret = luaL_dofile(L, "callback.lua");
    if(ret != 0)
    {
        printf("%s", lua_tostring(L, -1));
    }

    int i = 0;
    while(i<100)
    {
        onConnected();
        ++i;
    }
    lua_close(L);
    return 1;
}
