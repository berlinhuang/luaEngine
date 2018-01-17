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


//    要有Logger::setLogLevel 不然 会报未定义
//
    Logger::setLogLevel(Logger::DEBUG);

    LuaClass<InetAddress>(L)
            .create<unsigned short, bool, bool>("InetAddress");

    LuaClass<EventLoop>(L)
            .create("EventLoop");


    LuaClass<ChatServer>(L)
            .create<EventLoop*, const InetAddress&>("ChatServer")  // 模板方法
            .def("start", &ChatServer::start)
            .def("onConnection", &ChatServer::onConnection);

    return 0;
}