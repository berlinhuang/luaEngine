//
// Created by root on 18-2-2.
//

#ifndef LUAENGINE_LUASTART_H
#define LUAENGINE_LUASTART_H
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include "lua_tinker.h"
#include "LuaServer.h"

#include <signal.h>
using namespace std;

extern LuaServer *g_server;
extern boost::function<void(const char*)> _onNewConnection;
extern boost::function<void(const char*, const char*)> _onMessage;

extern char AppProgArgs[];//Main.cpp -> LuaStart.cpp


static int startServer( lua_State* L );
static int setNewConnectionCallback(lua_State *L);
static int setMessageCallback(lua_State *L);
static int onMessage(lua_State *L, const char* tm, const char* msg);
static int onNewConnection(lua_State *L, const char* ip);

void LuaStart();

int luaopen_LuaServer(lua_State *L);



#endif //LUAENGINE_LUASTART_H
