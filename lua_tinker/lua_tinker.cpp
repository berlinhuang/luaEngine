// lua_tinker.cpp
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright (c) 2005-2007 Kwon-il Lee (zupet@hitel.net)
// 
// please check Licence.txt file for licence and legal issues.

#include <iostream>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include "lua_tinker.h"


/*---------------------------------------------------------------------------*/
/* init                                                                      */
/*---------------------------------------------------------------------------*/
void lua_tinker::init(lua_State *L)
{
	init_s64(L);
	init_u64(L);
}

/*---------------------------------------------------------------------------*/
/* __s64                                                                     */
/*---------------------------------------------------------------------------*/
//为uint64_t 准备的metatable
static int tostring_s64(lua_State *L)
{
	char temp[64];
	//%I64u这样古老的标签，
	sprintf(temp, "%lld", *(long long*)lua_topointer(L, 1));
	lua_pushstring(L, temp);
	return 1;
}

/*---------------------------------------------------------------------------*/
static int eq_s64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(long long)) == 0);
	return 1;
}

/*---------------------------------------------------------------------------*/
static int lt_s64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(long long)) < 0);
	return 1;
}

/*---------------------------------------------------------------------------*/
static int le_s64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(long long)) <= 0);
	return 1;
}

/*---------------------------------------------------------------------------*/
void lua_tinker::init_s64(lua_State *L)
{
	const char* name = "__s64";
	lua_pushstring(L, name);
	lua_newtable(L);

	lua_pushstring(L, "__name");
	lua_pushstring(L, name);
	lua_rawset(L, -3);

	lua_pushstring(L, "__tostring");
	lua_pushcclosure(L, tostring_s64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__eq");
	lua_pushcclosure(L, eq_s64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__lt");
	lua_pushcclosure(L, lt_s64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__le");
	lua_pushcclosure(L, le_s64, 0);
	lua_rawset(L, -3);

	lua_settable(L, LUA_GLOBALSINDEX);
}

/*---------------------------------------------------------------------------*/
/* __u64                                                                     */
/*---------------------------------------------------------------------------*/
static int tostring_u64(lua_State *L)
{
	char temp[64];
	sprintf(temp, "%lld", *(unsigned long long*)lua_topointer(L, 1));
	lua_pushstring(L, temp);
	return 1;
}

/*---------------------------------------------------------------------------*/
static int eq_u64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(unsigned long long)) == 0);
	return 1;
}

/*---------------------------------------------------------------------------*/
static int lt_u64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(unsigned long long)) < 0);
	return 1;
}

/*---------------------------------------------------------------------------*/
static int le_u64(lua_State *L)
{
	lua_pushboolean(L, memcmp(lua_topointer(L, 1), lua_topointer(L, 2), sizeof(unsigned long long)) <= 0);
	return 1;
}

/*---------------------------------------------------------------------------*/
void lua_tinker::init_u64(lua_State *L)
{
	const char* name = "__u64";
	lua_pushstring(L, name);
	lua_newtable(L);

	lua_pushstring(L, "__name");
	lua_pushstring(L, name);
	lua_rawset(L, -3);

	lua_pushstring(L, "__tostring");
	lua_pushcclosure(L, tostring_u64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__eq");
	lua_pushcclosure(L, eq_u64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__lt");
	lua_pushcclosure(L, lt_u64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__le");
	lua_pushcclosure(L, le_u64, 0);
	lua_rawset(L, -3);

	lua_settable(L, LUA_GLOBALSINDEX);
}

/*---------------------------------------------------------------------------*/
/* excution                                                                  */
/*---------------------------------------------------------------------------*/
void lua_tinker::dofile(lua_State *L, const char *filename)
{
	//放入错误处理函数并且，记录堆栈地址
	lua_pushcclosure(L, on_error, 0);
	int errfunc = lua_gettop(L);
	//luaL_loadfile把一个文件加载为 Lua 代码块,这个函数使用 lua_load 加载文件中的数据,不运行
	//如果没有错误， lua_load 把一个编译好的代码块作为一个 Lua 函数压到栈顶。 否则，压入错误消息
	if (luaL_loadfile(L, filename) == 0)
	{
		if (lua_pcall(L, 0, 0, errfunc) != 0)//0个参数，0个返回值，返回0表示成功
		{
			lua_pop(L, 1);//失败出栈1个元素
		}
	}
	else
	{
		print_error(L, "%s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	//移除错误的string
	lua_pop(L, 1);
}

/*---------------------------------------------------------------------------*/
void lua_tinker::dostring(lua_State *L, const char* buff)
{
	lua_tinker::dobuffer(L, buff, strlen(buff));
}

/*---------------------------------------------------------------------------*/
void lua_tinker::dobuffer(lua_State *L, const char* buff, size_t len)
{
	lua_pushcclosure(L, on_error, 0);
	int errfunc = lua_gettop(L);

	if (luaL_loadbuffer(L, buff, len, "lua_tinker::dobuffer()") == 0)
	{
		if (lua_pcall(L, 0, 0, errfunc) != 0)
		{
			lua_pop(L, 1);
		}
	}
	else
	{
		print_error(L, "%s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	lua_pop(L, 1);
}

/*---------------------------------------------------------------------------*/
/* debug helpers                                                             */
/*---------------------------------------------------------------------------*/
static void call_stack(lua_State* L, int n)
{
	lua_Debug ar;
	if (lua_getstack(L, n, &ar) == 1)
	{
		lua_getinfo(L, "nSlu", &ar);

		const char* indent;
		if (n == 0)
		{
			indent = "->\t";
			lua_tinker::print_error(L, "\t<call stack>");
		}
		else
		{
			indent = "\t";
		}

		if (ar.name)
			lua_tinker::print_error(L, "%s%s() : line %d [%s : line %d]", indent, ar.name, ar.currentline, ar.source, ar.linedefined);
		else
			lua_tinker::print_error(L, "%sunknown : line %d [%s : line %d]", indent, ar.currentline, ar.source, ar.linedefined);

		call_stack(L, n + 1);
	}
}

/*---------------------------------------------------------------------------*/
int lua_tinker::on_error(lua_State *L)
{
	print_error(L, "%s", lua_tostring(L, -1));

	call_stack(L, 0);

	return 0;
}

/*---------------------------------------------------------------------------*/
void lua_tinker::print_error(lua_State *L, const char* fmt, ...)
{
	char text[4096];

	va_list args;
	va_start(args, fmt);
	vsprintf(text, fmt, args);
	va_end(args);
	//可以注册这个 __ALERT名称对应的函数，用于错误处理
	lua_pushstring(L, "_ALERT");
	lua_gettable(L, LUA_GLOBALSINDEX);
	if (lua_isfunction(L, -1))
	{
		lua_pushstring(L, text);
		lua_call(L, 1, 0);
	}
	else
	{
		printf("%s\n", text);
		//如果弹出的不是函数，
		lua_pop(L, 1);
	}
}

/*---------------------------------------------------------------------------*/
void lua_tinker::enum_stack(lua_State *L)
{
	int top = lua_gettop(L);
	print_error(L, "Type:%d", top);
	for (int i = 1; i <= lua_gettop(L); ++i)
	{
		switch (lua_type(L, i))
		{
			case LUA_TNIL:
				print_error(L, "\t%s", lua_typename(L, lua_type(L, i)));
				break;
			case LUA_TBOOLEAN:
				print_error(L, "\t%s	%s", lua_typename(L, lua_type(L, i)), lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TLIGHTUSERDATA:
				print_error(L, "\t%s	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				break;
			case LUA_TNUMBER:
				print_error(L, "\t%s	%f", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				print_error(L, "\t%s	%s", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));
				break;
			case LUA_TTABLE:
				print_error(L, "\t%s	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				break;
			case LUA_TFUNCTION:
				print_error(L, "\t%s()	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				break;
			case LUA_TUSERDATA:
				print_error(L, "\t%s	0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
				break;
			case LUA_TTHREAD:
				print_error(L, "\t%s", lua_typename(L, lua_type(L, i)));
				break;
		}
	}
}

/*---------------------------------------------------------------------------*/
/* read                                                                      */
/*---------------------------------------------------------------------------*/
template<>
char* lua_tinker::read(lua_State *L, int index)
{
	return (char*)lua_tostring(L, index);
}

template<>
const char* lua_tinker::read(lua_State *L, int index)
{
	return (const char*)lua_tostring(L, index);
}

template<>
char lua_tinker::read(lua_State *L, int index)
{
	return (char)lua_tonumber(L, index);
}

template<>
unsigned char lua_tinker::read(lua_State *L, int index)
{
	return (unsigned char)lua_tonumber(L, index);
}

template<>
short lua_tinker::read(lua_State *L, int index)
{
	return (short)lua_tonumber(L, index);
}

template<>
unsigned short lua_tinker::read(lua_State *L, int index)
{
	return (unsigned short)lua_tonumber(L, index);
}

template<>
long lua_tinker::read(lua_State *L, int index)
{
	return (long)lua_tonumber(L, index);
}

template<>
unsigned long lua_tinker::read(lua_State *L, int index)
{
	return (unsigned long)lua_tonumber(L, index);
}

template<>
int lua_tinker::read(lua_State *L, int index)
{
	return (int)lua_tonumber(L, index);
}

template<>
unsigned int lua_tinker::read(lua_State *L, int index)
{
	return (unsigned int)lua_tonumber(L, index);
}

template<>
float lua_tinker::read(lua_State *L, int index)
{
	return (float)lua_tonumber(L, index);
}

template<>
double lua_tinker::read(lua_State *L, int index)
{
	return (double)lua_tonumber(L, index);
}

template<>
bool lua_tinker::read(lua_State *L, int index)
{
	if (lua_isboolean(L, index))
		return lua_toboolean(L, index) != 0;
	else
		return lua_tonumber(L, index) != 0;
}

template<>
void lua_tinker::read(lua_State *L, int index)
{
	return;
}

template<>
long long lua_tinker::read(lua_State *L, int index)
{
	if (lua_isnumber(L, index))
		return (long long)lua_tonumber(L, index);
	else
		return *(long long*)lua_touserdata(L, index);
}
template<>
unsigned long long lua_tinker::read(lua_State *L, int index)
{
	if (lua_isnumber(L, index))
		return (unsigned long long)lua_tonumber(L, index);
	else
		return *(unsigned long long*)lua_touserdata(L, index);
}

template<>
lua_tinker::table lua_tinker::read(lua_State *L, int index)
{
	return table(L, index);
}

/*---------------------------------------------------------------------------*/
/* push                                                                      */
/*---------------------------------------------------------------------------*/
template<>
void lua_tinker::push(lua_State *L, char ret)
{
	lua_pushnumber(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, unsigned char ret)
{
	lua_pushnumber(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, short ret)
{
	lua_pushnumber(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, unsigned short ret)
{
	lua_pushnumber(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, long ret)
{
	lua_pushnumber(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, unsigned long ret)
{
	lua_pushnumber(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, int ret)
{
	lua_pushnumber(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, unsigned int ret)
{
	lua_pushnumber(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, float ret)
{
	lua_pushnumber(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, double ret)
{
	lua_pushnumber(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, char* ret)
{
	lua_pushstring(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, const char* ret)
{
	lua_pushstring(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, bool ret)
{
	lua_pushboolean(L, ret);
}

template<>
void lua_tinker::push(lua_State *L, lua_value* ret)
{
	if (ret) ret->to_lua(L); else lua_pushnil(L);
}

template<>
void lua_tinker::push(lua_State *L, long long ret)
{
	*(long long*)lua_newuserdata(L, sizeof(long long)) = ret;
	lua_pushstring(L, "__s64");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_setmetatable(L, -2);
}
template<>
void lua_tinker::push(lua_State *L, unsigned long long ret)
{
	*(unsigned long long*)lua_newuserdata(L, sizeof(unsigned long long)) = ret;
	lua_pushstring(L, "__u64");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_setmetatable(L, -2);
}

template<>
void lua_tinker::push(lua_State *L, lua_tinker::table ret)
{
	lua_pushvalue(L, ret.m_obj->m_index);
}

//添加一个函数,支持字符串的push
//template<>
//void lua_tinker::push(lua_State *L, std::string ret )
//{
//	lua_pushstring( L, ret.c_str() );
//}


/*---------------------------------------------------------------------------*/
/* pop                                                                       */
/*---------------------------------------------------------------------------*/
template<>
void lua_tinker::pop(lua_State *L)
{
	lua_pop(L, 1);
}

template<>
lua_tinker::table lua_tinker::pop(lua_State *L)
{
	return table(L, lua_gettop(L));
}

/*---------------------------------------------------------------------------*/
/* Tinker Class Helper                                                       */
/*---------------------------------------------------------------------------*/
static void invoke_parent(lua_State *L)
{
	lua_pushstring(L, "__parent");
	lua_rawget(L, -2);
	if (lua_istable(L, -1))
	{
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		if (!lua_isnil(L, -1))
		{
			lua_remove(L, -2);
		}
		else
		{
			lua_remove(L, -1);
			invoke_parent(L);
			lua_remove(L, -2);
		}
	}
}

/*---------------------------------------------------------------------------*/
//LUA的程序通过这个函数完定义类的__index
//这个函数的堆栈情况可以这样理解
//第一次enum_stack时
//-1,结果
//-2，key
//-3，metatable
//-4,key
int lua_tinker::meta_get(lua_State *L)
{

	lua_getmetatable(L, 1);	//要检查的数据在栈底部，取出得到其metatable放入栈顶
	lua_pushvalue(L, 2);    //复制栈底倒数第二个参数key，放入栈顶
	lua_rawget(L, -2);    	//在metatable里面寻找key，

	if (lua_isuserdata(L, -1))  //如果是一个userdata
	{
		user2type<var_base*>::invoke(L, -1)->get(L);        //进行调用
		lua_remove(L, -2);       							//从堆栈移除userdata.
	}
	else if (lua_isnil(L, -1))  //如果没有找到
	{
		lua_remove(L, -1);
		invoke_parent(L);       //检查的他的父类里面是否有可以调用的
		if (lua_isnil(L, -1))   //如果仍然是NULL
		{
			lua_pushfstring(L, "can't find '%s' class variable. (forgot registering class variable ?)", lua_tostring(L, 2));
			lua_error(L);
		}
		//这儿是有问题的，其实没有调用父类的数据。
	}

	lua_remove(L, -2);    //删除掉metatable，

	return 1;
}

/*---------------------------------------------------------------------------*/
//LUA的程序通过这个函数完成定义类的__newindex
int lua_tinker::meta_set(lua_State *L)
{
	lua_getmetatable(L, 1);
	lua_pushvalue(L, 2);
	lua_rawget(L, -2);

	if (lua_isuserdata(L, -1))
	{
		user2type<var_base*>::invoke(L, -1)->set(L);
	}
	else if (lua_isnil(L, -1))
	{
		lua_pushvalue(L, 2);
		lua_pushvalue(L, 3);
		lua_rawset(L, -4);
	}
	lua_settop(L, 3);
	return 0;
}

/*---------------------------------------------------------------------------*/
void lua_tinker::push_meta(lua_State *L, const char* name)
{
	lua_pushstring(L, name);
	lua_gettable(L, LUA_GLOBALSINDEX);//t[k]压栈   t是索引指向的值，k是栈顶放的值，全局表[lua_globalsindex][name] 压栈
}

/*---------------------------------------------------------------------------*/
/* table object on stack                                                     */
/*---------------------------------------------------------------------------*/
lua_tinker::table_obj::table_obj(lua_State* L, int index)
		:m_L(L)
		, m_index(index)
		, m_ref(0)
{
	m_pointer = lua_topointer(m_L, m_index);
}

lua_tinker::table_obj::~table_obj()
{
	if (validate())
	{
		lua_remove(m_L, m_index);
	}
}

void lua_tinker::table_obj::inc_ref()
{
	++m_ref;
}

void lua_tinker::table_obj::dec_ref()
{
	if (--m_ref == 0)
		delete this;
}

bool lua_tinker::table_obj::validate()
{
	if (m_pointer != NULL)
	{
		if (m_pointer == lua_topointer(m_L, m_index))
		{
			return true;
		}
		else
		{
			int top = lua_gettop(m_L);

			for (int i = 1; i <= top; ++i)
			{
				if (m_pointer == lua_topointer(m_L, i))
				{
					m_index = i;
					return true;
				}
			}

			m_pointer = NULL;
			return false;
		}
	}
	else
	{
		return false;
	}
}

/*---------------------------------------------------------------------------*/
/* Table Object Holder                                                       */
/*---------------------------------------------------------------------------*/
lua_tinker::table::table(lua_State* L)
{
	lua_newtable(L);

	m_obj = new table_obj(L, lua_gettop(L));
	//原来作者这个地方写漏了一个引用增加
	m_obj->inc_ref();
}

lua_tinker::table::table(lua_State* L, const char* name)
{
	lua_pushstring(L, name);
	lua_gettable(L, LUA_GLOBALSINDEX);

	if (lua_istable(L, -1) == 0)
	{
		lua_pop(L, 1);

		lua_newtable(L);
		lua_pushstring(L, name);
		lua_pushvalue(L, -2);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	m_obj = new table_obj(L, lua_gettop(L));
}

lua_tinker::table::table(lua_State* L, int index)
{
	if (index < 0)
	{
		index = lua_gettop(L) + index + 1;
	}

	m_obj = new table_obj(L, index);

	m_obj->inc_ref();
}

lua_tinker::table::table(const table& input)
{
	m_obj = input.m_obj;

	m_obj->inc_ref();
}

lua_tinker::table::~table()
{
	m_obj->dec_ref();
}

/*---------------------------------------------------------------------------*/
