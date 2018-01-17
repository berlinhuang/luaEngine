//
// Created by root on 18-1-17.
//


extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include "lua_tinker.h"
#include "lua_bind.h"
#include "luaserver.h"
using namespace muduo::net;



int main()
{

    lua_State* L = lua_open();
    luaopen_base(L);//基础库
    luaopen_string(L);//字符串库


//    要有实现代码 单单头文件+lib会报未定义
//

//    LuaClass<InetAddress>(L)
//            .create<unsigned short, bool, bool>("InetAddress");
//
//    LuaClass<EventLoop>(L)
//            .create("EventLoop");


//    LuaClass<ChatServer>(L)
//            .create<EventLoop,InetAddress>("ChatServer")
//            .def("start", &ChatServer::start)
//            .def("onConnection", &ChatServer::onConnection);

    return 0;
}