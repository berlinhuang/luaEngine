//
// Created by root on 18-1-17.
//

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include <signal.h>
#include "lua_bind.h"
#include "luaserver.h"
using namespace std;

void startServer( char* ip, int port )
{
    EventLoop loop;
    InetAddress serverAddr(ip, port);

    muduo::Thread thr(boost::bind(hanleMessageQueue), "hanleMessageQueue");
    thr.start();

    ChatServer server(&loop, serverAddr);
    g_server = &server;
    server.start();

    loop.loop();
    thr.join();
}

static char AppProgArgs[] = "";//全局变量

int ScriptSys_exit( lua_State* L )
{
    int nArgc = lua_gettop( L ) ;
    if( nArgc == 0 )
        return 0 ;
    int32_t nNum = luaL_checknumber( L , 1 ) ;
    lua_pushnumber( L , nNum ) ;
    if(nNum==-1)
    {
        lua_close(L);
    }
    return 1 ;
}

static const struct luaL_Reg stCommonFunc[] = {
        { "exit"  , ScriptSys_exit } ,
        { NULL , NULL }
};

int cpp_func(int arg1, int arg2)
{
    return arg1 + arg2;
}



int main(int argc, char* argv[])
{
#ifndef _WIN32
    //! linux下需要屏蔽的一些信号
    signal( SIGHUP, SIG_IGN );
    signal( SIGALRM, SIG_IGN );
    signal( SIGPIPE, SIG_IGN );
    signal( SIGXCPU, SIG_IGN );
    signal( SIGXFSZ, SIG_IGN );
    signal( SIGPROF, SIG_IGN );
    signal( SIGVTALRM, SIG_IGN );
    signal( SIGQUIT, SIG_IGN );
    signal( SIGCHLD, SIG_IGN);
#endif
    if (strcmp(argv[1], "--help") == 0|| strcmp(argv[1], "/?") == 0)
    {
        cout << "----------------------usage:" << endl;
        cout << "--help or /? show usage" << endl;
        cout << "-f run as a foreground program, accept input command" << endl;
        cout << "-v show build time and svn version" << endl;
        cout << "-s [WorldServer|SceneServer|DBServer] run the specific server" << endl;
        cout << "egg: ./server -f -s SceneServer" << endl;
        return 0;
    }

    for(int i = 1; i<argc; i++ )
    {
        strcat(AppProgArgs, argv[i]);
        strcat(AppProgArgs," ");
    }

    Logger::setLogLevel(Logger::INFO);//要有Logger::setLogLevel 不然 会报未定义
    lua_State* L = lua_open();
    luaopen_base(L);//基础库
    luaopen_string(L);//字符串库
    lua_tinker::set(L, "g_server", g_server);//没注册 这个 为什么会有个signal
    lua_tinker::set(L, "AppProgArgs", AppProgArgs);
    lua_tinker::def(L, "cpp_func", cpp_func);
    lua_tinker::def(L, "startServer", startServer);

    luaL_register( L , "ScriptSys" , stCommonFunc ) ;

    lua_tinker::dofile(L, "luaserver.lua");
    lua_close(L);
    return 0;
}






//    LuaClass<InetAddress>(L)
////            .create<unsigned short, bool, bool>("InetAddress");
//              .create<StringArg, unsigned short, bool>("InetAddress");
//
//
//    LuaClass<EventLoop>(L)
//            .create("EventLoop")
//            .def("loop",&EventLoop::loop);
//
//    LuaClass<ChatServer>(L)
//            .create<EventLoop*, const InetAddress&>("ChatServer")  // 模板方法
//            .def("start", &ChatServer::start)
//            .def("onConnection", &ChatServer::onConnection);
