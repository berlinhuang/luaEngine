//
// Created by root on 18-1-16.
//

#include "sample.h"

test g_t(11);

bool luaBindClass(lua_State* state)
{
    LuaClass<base>(state)
            .create("base")
            .def("is_base", &base::is_base);

//    LuaClass<test,base>(state).create_inh("test")// 包含了 相面三条语句实现
                    lua_tinker::class_add<test>(state, "test");
                    lua_tinker::class_inh<test, base>(state);//注册类继承关系
                    lua_tinker::class_con<test>(state, lua_tinker::constructor<test, int>); //cons
                    LuaClass<test>(state)
            .def("is_test", &test::is_test)
            .def("ret_void", &test::ret_void)
            .def("ret_int", &test::ret_int)
            .def("ret_mul", &test::ret_mul)
            .def("get", &test::get)
            .def("set", &test::set)
            .mem("_test", &test::_test);

    //lua_tinker::class_mem<test>(state, "_test", &test::_test);//成员
    lua_tinker::set(state, "g_test", &g_t);//注册全局变量
    return true;
}


int main()
{
    lua_State* L = lua_open();
    luaopen_base(L);//基础库
    luaopen_string(L);//字符串库
    luaBindClass(L);
    lua_tinker::dofile(L, "sample.lua");

    lua_close(L);
    return 0;
}