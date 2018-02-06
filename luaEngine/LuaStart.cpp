//
// Created by root on 18-2-2.
//

#include "LuaStart.h"

int startServer( lua_State* L )
{
    const char *ip = luaL_checkstring(L,1);
    if( ! ip ) return 0;
    int port = luaL_checkint(L,2);

    EventLoop loop;
    InetAddress serverAddr(ip, port);
    muduo::Thread thr(boost::bind(LuaServer::hanleMessageQueue), "hanleMessageQueue");
    thr.start();
    LuaServer server(&loop, serverAddr);
    g_server = &server;
    server.start();

    loop.loop();
    thr.join();
    return 0;
}

static int lua_messgecallback = LUA_REFNIL;
static int lua_newconnectioncallback = LUA_REFNIL;

int setNewConnectionCallback(lua_State *L)
{
    lua_newconnectioncallback = luaL_ref(L, LUA_REGISTRYINDEX);
    return 0;
}

int setMessageCallback(lua_State *L)
{
    lua_messgecallback = luaL_ref(L, LUA_REGISTRYINDEX);// 对栈顶的值v生成返回一个引用r，即将[引用r， 栈顶值v]存到LUA_REGISTRYINDEX表中
    return 0;
}

int onMessage(lua_State *L, const char* tm, const char* msg)
{
    // 将一个引用值入栈
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_messgecallback);
    lua_pushstring(L, tm);
    lua_pushstring(L, msg);
    int status = lua_pcall(L, 2, 0, 1);//L当前栈，参数个数，返回值个数，发生错误处理时代码返回
}

int onNewConnection(lua_State *L, const char* ip)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_newconnectioncallback);
    lua_pushstring(L, ip);//params
    int status = lua_pcall(L, 1, 0, 1);//L当前栈，参数个数，返回值个数，发生错误处理时代码返回
}

//{  name,  func  }
//{ 字符串， c函数指针 }
static struct luaL_Reg stLuaServer[] = {
        { "_startServer", startServer},
        { "_setNewConnectionCallback", setNewConnectionCallback },
        { "_setMessageCallback", setMessageCallback },
        { NULL, NULL }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int sysExit( lua_State* L )
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

static const struct luaL_Reg stFunc[] = {
        { "_exit"  , sysExit } ,
        { NULL , NULL }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void exportClass( lua_State* L )
{
    LuaClass<LuaServer>(L)
            .create<EventLoop*, const InetAddress&>("ChatServer")  // 模板方法
            .def("start", &LuaServer::start)
            .def("onConnection", &LuaServer::onConnection);

    LuaClass<InetAddress>(L)
            .create<unsigned short, bool, bool>("InetAddress")
            .create<StringArg, unsigned short, bool>("InetAddress");

    LuaClass<EventLoop>(L)
            .create("EventLoop")
            .def("loop",&EventLoop::loop);

    LuaClass<muduo::Thread>(L)
            .create<boost::function<void ()>, string >("Thread")
            .def("start",&muduo::Thread::start);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////




void LuaStart()
{

    lua_State* L = lua_open();
    luaL_openlibs(L);

//导出全局变量
    lua_tinker::set(L, "g_server", g_server);//没注册 这个 为什么会有个signal
    lua_tinker::set(L, "g_progArgs", AppProgArgs);

//导出函数
    luaL_register( L , "CFSys" , stFunc ) ;
    luaL_register( L , "CFServer" , stLuaServer ) ;

    _onNewConnection = boost::bind(onNewConnection, L, _1);
    _onMessage = boost::bind(onMessage, L,_1,_2);

    lua_tinker::dofile(L, "ServMain.lua");
    lua_close(L);
}


























//导出全局表格CT...(C Table)
//    lua_newtable(L);
//    lua_tinker::table luaserver(L, "CTTest");
//    luaserver.set("val", 1);
//    luaserver.set("sub", lua_tinker::table(L));
//    lua_newtable(L);
//    lua_tinker::table sub = luaserver.get<lua_tinker::table>("sub");//获取sub = { }
//    sub.set("val", 2);//给表变量赋值 CTTest = { val = 1, sub = { val =2 } }
//    lua_settable(L, LUA_GLOBALSINDEX);
//    lua_settable(L, LUA_GLOBALSINDEX);




//导出函数CF...(C Func)
//    lua_newtable(L);
//    lua_tinker::subdef(L, "cppFunc", cppFunc);
//    lua_tinker::subdef(L, "startServer", startServer);
//    lua_setglobal(L, "CFServer");



//
//int luaopen_LuaServer(lua_State *L)
//{
//    lua_newtable(L);
//    for (luaL_Reg *l = stLuaServer; l->name != NULL; l++) {
//        //lua_push*族函数都有"创建一个类型的值并压入"的语意
//        //lua_pushcclosure(L, c函数指针, 函数关联的upvalue的个数)
//        lua_pushcclosure(L, l->func, 0);  /* closure with those upvalues */
//        lua_setfield(L, -2, l->name);// lua_setfield( L, 索引, 字符串键 )    索引到t    t[l->name]= l->func   并把l->func出栈
//    }
//    lua_setglobal(L, "luaserver");//将栈顶元素（newtable）置为Lua环境中的全局变量luaserver
//    return 0;
//}