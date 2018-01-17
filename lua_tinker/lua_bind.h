//
// Created by root on 18-1-16.
//

#ifndef CHAT_LUA_BIND_H
#define CHAT_LUA_BIND_H

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include "lua_tinker.h"

template< typename T >
void RegisterMyFunc(lua_State* state, const char* func_name, lua_CFunction f)
{
    lua_tinker::push_meta(state, lua_tinker::class_name<T>::name());
    lua_pushstring(state, func_name);
    lua_pushcfunction(state, f);
    lua_settable(state, -3);
    lua_pop(state, 1);
}


int my_getpoint(lua_State* L);
bool customLuaLoader(lua_State* L);

void Register();

template<typename Object, typename ...Arg>
class LuaClass
{
    lua_State*  m_state;
    bool		m_bCreated;
public:
    LuaClass(lua_State* state)
    {
        m_state = state;
        m_bCreated = false;
    }

    template<typename Func>
    inline LuaClass& def(const char* name, Func func)
    {
        lua_tinker::class_def<Object>(m_state, name, func);
        return *this;
    }

    inline LuaClass& def2(const char* name, lua_CFunction func)
    {
        RegisterMyFunc<Object>(m_state, name, func);
        return *this;
    }

    //新增绑定对象成员 &class::mem
    template<typename BASE, typename VAR>
    inline LuaClass& mem(const char* name, VAR BASE::*val)
    {
        lua_tinker::class_mem<Object>(m_state, name, val);
        return *this;
    }

    inline LuaClass& class_add(const char* name)
    {
        lua_tinker::class_add<Object>(m_state, name);
        return *this;
    }

    inline LuaClass& def_cpoint(const char* name = NULL)
    {
        if (name == NULL || !name)
        {
            return def2("cpoint", my_getpoint);
        }

        return def2(name, my_getpoint);
    }

    //新增 添加+继承+构造
    inline LuaClass& create_inh(const char* name)
    {
        if (!m_bCreated)
        {
            lua_tinker::class_add<Object>(m_state, name);
            m_bCreated = true;
        }
        lua_tinker::class_inh<Object, Arg...>(m_state);//注册类继承关系
        lua_tinker::class_con<Object>(m_state, lua_tinker::constructor<Object,int>);
        return *this;
    }

    inline LuaClass& create(const char* name)
    {
        if (!m_bCreated)
        {
            lua_tinker::class_add<Object>(m_state, name);
            m_bCreated = true;
        }
        lua_tinker::class_con<Object>(m_state, lua_tinker::constructor<Object>);
        return *this;
    }

    template<typename T1>
    inline LuaClass& create(const char* name)
    {
        if (!m_bCreated)
        {
            lua_tinker::class_add<Object>(m_state, name);
            m_bCreated = true;
        }
        lua_tinker::class_con<Object>(m_state, lua_tinker::constructor<Object, T1>);
        return *this;
    }
    template<typename T1, typename T2>
    inline LuaClass& create(const char* name)
    {
        if (!m_bCreated)
        {
            lua_tinker::class_add<Object>(m_state, name);
            m_bCreated = true;
        }
        lua_tinker::class_con<Object>(m_state, lua_tinker::constructor<Object, T1, T2>);
        return *this;
    }

    template<typename T1, typename T2, typename T3>
    inline LuaClass& create(const char* name)
    {
        if (!m_bCreated)
        {
            lua_tinker::class_add<Object>(m_state, name);
            m_bCreated = true;
        }
        lua_tinker::class_con<Object>(m_state, lua_tinker::constructor<Object, T1, T2, T3>);
        return *this;
    }
};



#endif //CHAT_LUA_BIND_H
