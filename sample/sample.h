//
// Created by root on 18-1-16.
//

#ifndef CHAT_SAMPLE_H
#define CHAT_SAMPLE_H

#include "lua_bind.h"

bool luaBindClass(lua_State* state);
void lua_bind_sample();

struct A
{
    A(int v) : value(v) {}//构造 给value赋值 value = v
    int value;
};

struct base
{
    base() {}//构造
    const char* is_base(){ return "this is base"; }
};

class test : public base
{
public:
    test(int val) : _test(val) {}//构造 给成员变量赋值 _test = val
    ~test() {}//析构
    const char* is_test(){ return "this is test"; }
    void ret_void() {}
    int ret_int()				{ return _test; }// get _test
    int ret_mul(int m) const	{ return _test * m; }// get _test*m
    A get()						{ return A(_test); }// 返回实例化对象A
    void set(A a)				{ _test = a.value; }// set _test = A.value
    int _test;
};

#endif //CHAT_SAMPLE_H
