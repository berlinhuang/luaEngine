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

static char AppProgArgs[] = "";//全局变量






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

int cppFunc(int arg1, int arg2)
{
    return arg1 + arg2;
}


//{  name,  func  }
//{ 字符串， c函数指针 }
static luaL_Reg luaserver[] = {
        { NULL, NULL }
};

int luaopen_luaserver(lua_State *L)
{
    lua_newtable(L);
    for (luaL_Reg *l = luaserver; l->name != NULL; l++) {
        //lua_push*族函数都有"创建一个类型的值并压入"的语意
        //lua_pushcclosure(L, c函数指针, 函数关联的upvalue的个数)
        lua_pushcclosure(L, l->func, 0);  /* closure with those upvalues */
        lua_setfield(L, -2, l->name);// lua_setfield( L, 索引, 字符串键 )    索引到t    t[l->name]= l->func   并把l->func出栈
    }
    lua_setglobal(L, "luaserver");//将栈顶元素（newtable）置为Lua环境中的全局变量luaserver
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////////


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
    luaL_openlibs(L);

//导出全局变量
    lua_tinker::set(L, "g_server", g_server);//没注册 这个 为什么会有个signal
    lua_tinker::set(L, "AppProgArgs", AppProgArgs);

////导出全局表格CT...(C Table)
//    lua_tinker::table luaserver(L, "CTTest");
//    luaserver.set("key", 1);
//    luaserver.set("sub", lua_tinker::table(L));
// TODO:



//导出函数CF...(C Func)
    lua_newtable(L);
    lua_tinker::subdef(L, "cppFunc", cppFunc);
    lua_tinker::subdef(L, "startServer", startServer);
    lua_setglobal(L, "CFServer");

//导出函数
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
