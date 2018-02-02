// lua_tinker.h
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright (c) 2005-2007 Kwon-il Lee (zupet@hitel.net)
//
// please check Licence.txt file for licence and legal issues.

#if !defined(_LUA_TINKER_H_)
#define _LUA_TINKER_H_

#include <new>
#include <string.h>


namespace lua_tinker
{
    // init LuaTinker
    void	init(lua_State *L);

    void	init_s64(lua_State *L);
    void	init_u64(lua_State *L);

    // excution
    void	dofile(lua_State *L, const char *filename);
    void	dostring(lua_State *L, const char* buff);
    void	dobuffer(lua_State *L, const char* buff, size_t sz);

    // debug helpers
    void	enum_stack(lua_State *L);
    int		on_error(lua_State *L);
    void	print_error(lua_State *L, const char* fmt, ...);

    // dynamic type extention
    struct lua_value
    {
        virtual void to_lua(lua_State *L) = 0;
    };

    // type trait
    template<typename T> struct class_name;
    struct table;

    template<bool C, typename A, typename B> struct if_ {};
    template<typename A, typename B>		struct if_<true, A, B> { typedef A type; };
    template<typename A, typename B>		struct if_<false, A, B> { typedef B type; };

    template<typename A>
    struct is_ptr { static const bool value = false; };
    template<typename A>
    struct is_ptr<A*> { static const bool value = true; };

    template<typename A>
    struct is_ref { static const bool value = false; };
    template<typename A>
    struct is_ref<A&> { static const bool value = true; };

    template<typename A>
    struct remove_const { typedef A type; };
    template<typename A>
    struct remove_const<const A> { typedef A type; };

    template<typename A>
    struct base_type { typedef A type; };
    template<typename A>
    struct base_type<A*> { typedef A type; };
    template<typename A>
    struct base_type<A&> { typedef A type; };

    template<typename A>
    struct class_type { typedef typename remove_const<typename base_type<A>::type>::type type; };

    /////////////////////////////////
    enum { no = 1, yes = 2 };
    typedef char(&no_type)[no];
    typedef char(&yes_type)[yes];

    struct int_conv_type { int_conv_type(int); };

    no_type int_conv_tester(...);
    yes_type int_conv_tester(int_conv_type);

    no_type vfnd_ptr_tester(const volatile char *);
    no_type vfnd_ptr_tester(const volatile short *);
    no_type vfnd_ptr_tester(const volatile int *);
    no_type vfnd_ptr_tester(const volatile long *);
    no_type vfnd_ptr_tester(const volatile double *);
    no_type vfnd_ptr_tester(const volatile float *);
    no_type vfnd_ptr_tester(const volatile bool *);
    yes_type vfnd_ptr_tester(const volatile void *);

    template <typename T> T* add_ptr(T&);

    template <bool C> struct bool_to_yesno { typedef no_type type; };
    template <> struct bool_to_yesno<true> { typedef yes_type type; };

    template <typename T>
    struct is_enum
    {
        static T arg;
        static const bool value = ((sizeof(int_conv_tester(arg)) == sizeof(yes_type)) && (sizeof(vfnd_ptr_tester(add_ptr(arg))) == sizeof(yes_type)));
    };
    /////////////////////////////////

    // from lua
    template<typename T>
    struct void2val { static T invoke(void* input){ return *(T*)input; } };
    template<typename T>
    struct void2ptr { static T* invoke(void* input){ return (T*)input; } };
    template<typename T>
    struct void2ref { static T& invoke(void* input){ return *(T*)input; } };
//根据T的类型，决定将void *进行什么样的转换
    template<typename T>
    struct void2type
    {
        static T invoke(void* ptr)
        {
            //if_有多层嵌套，你可以认为就是if  else{ if  else }
            return	if_<is_ptr<T>::value
                    , void2ptr<typename base_type<T>::type>
                    , typename if_<is_ref<T>::value
                            , void2ref<typename base_type<T>::type>
                            , void2val<typename base_type<T>::type>
                    >::type
            >::type::invoke(ptr);
        }
    };

    struct user
    {
        user(void* p) : m_p(p) {}
        virtual ~user() {}
        void* m_p;
    };

    //将用户push的user data信息进行转换，push light data 是void *
    template<typename T>
    struct user2type { static T invoke(lua_State *L, int index) { return void2type<T>::invoke(lua_touserdata(L, index)); } };

    template<typename T>
    struct lua2enum { static T invoke(lua_State *L, int index) { return (T)(int)lua_tonumber(L, index); } };

    template<typename T>
    struct lua2object
    {
        static T invoke(lua_State *L, int index)
        {
            if (!lua_isuserdata(L, index))
            {
                lua_pushstring(L, "no class at first argument. (forgot ':' expression ?)");
                lua_error(L);
            }
            return void2type<T>::invoke(user2type<user*>::invoke(L, index)->m_p);
        }
    };

    template<typename T>
    T lua2type(lua_State *L, int index)
    {
        return	if_<is_enum<T>::value
                , lua2enum<T>
                , lua2object<T>
        >::type::invoke(L, index);
    }

    //T是注册的类型
    //从user继承的类，用于帮助完成构造类，同时通过user基类提供析构释放的方法，
    //非要用这种转换的方式，而不直接用T，原因是？
    //原来我的感觉是因为为了变参的构造函数，感觉在C11下没有必要？C11可以直接完成变参了。
    //后来发现其实其还对val，ptr，ref做了考虑， 当然这个考虑未必完整。
    template<typename T>
    struct val2user : user
    {
        val2user() : user(new T) {}

        template<typename T1>
        val2user(T1 t1) : user(new T(t1)) {}

        template<typename T1, typename T2>
        val2user(T1 t1, T2 t2) : user(new T(t1, t2)) {}

        template<typename T1, typename T2, typename T3>
        val2user(T1 t1, T2 t2, T3 t3) : user(new T(t1, t2, t3)) {}

        template<typename T1, typename T2, typename T3, typename T4>
        val2user(T1 t1, T2 t2, T3 t3, T4 t4) : user(new T(t1, t2, t3, t4)) {}

        template<typename T1, typename T2, typename T3, typename T4, typename T5>
        val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) : user(new T(t1, t2, t3, t4, t5)) {}
        //辅助完成析构,原作好像少了virtual
        ~val2user() { delete ((T*)m_p); }
    };

    template<typename T>
    struct ptr2user : user
    {
        ptr2user(T* t) : user((void*)t) {}
    };

    template<typename T>
    struct ref2user : user
    {
        ref2user(T& t) : user(&t) {}
    };

    //将一些结构转换给lua，其用了很多模板干活，
    //原来以为这层代码是可以简化的，但后来发现其实tinker对ptr，ref的考虑
    //是封装了指针给LUA，所以其对ptr，ref都有很多再次封装，这点上可能不能简化

    // to lua
    template<typename T>
    struct val2lua { static void invoke(lua_State *L, T& input){ new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(input); } };
    template<typename T>
    struct ptr2lua { static void invoke(lua_State *L, T* input){ if (input) new(lua_newuserdata(L, sizeof(ptr2user<T>))) ptr2user<T>(input); else lua_pushnil(L); } };
    template<typename T>
    struct ref2lua { static void invoke(lua_State *L, T& input){ new(lua_newuserdata(L, sizeof(ref2user<T>))) ref2user<T>(input); } };

    //enum只是被当做整数使用
    template<typename T>
    struct enum2lua { static void invoke(lua_State *L, T val) { lua_pushnumber(L, (int)val); } };


    // class helper
    int meta_get(lua_State *L);
    int meta_set(lua_State *L);
    void push_meta(lua_State *L, const char* name);

    //我不知道为什么要样这种方式转换object？直接使用特化函数不是一样可以达到效果吗？
    template<typename T>
    struct object2lua
    {
        static void invoke(lua_State *L, T val)
        {
            if_<is_ptr<T>::value
                    , ptr2lua<typename base_type<T>::type>
                    , typename if_<is_ref<T>::value
                            , ref2lua<typename base_type<T>::type>
                            , val2lua<typename base_type<T>::type>
                    >::type
            >::type::invoke(L, val);
            //注意这个地方，其通过class_type<T>::type 萃取出来实际的类型，
            //class_type会去掉引用等信息
            push_meta(L, class_name<typename class_type<T>::type>::name());
            lua_setmetatable(L, -2);
        }
    };

    template<typename T>
    void type2lua(lua_State *L, T val)
    {
        //如果是枚举，就用枚举，如果不是呢，就用object
        if_<is_enum<T>::value
                , enum2lua<T>
                , object2lua<T>
        >::type::invoke(L, val);
    }

    // get value from cclosure
    //得到函数的，upvalue，帮助完成函数调用
    //T 是用户的类型，
    //根据要转换的的T进行操作。
    template<typename T>
    T upvalue_(lua_State *L)
    {
        return user2type<T>::invoke(L, lua_upvalueindex(1));
    }

    // read a value from lua stack
    template<typename T>
    T read(lua_State *L, int index)				{ return lua2type<T>(L, index); }

    template<>	char*				read(lua_State *L, int index);
    template<>	const char*			read(lua_State *L, int index);
    template<>	char				read(lua_State *L, int index);
    template<>	unsigned char		read(lua_State *L, int index);
    template<>	short				read(lua_State *L, int index);
    template<>	unsigned short		read(lua_State *L, int index);
    template<>	long				read(lua_State *L, int index);
    template<>	unsigned long		read(lua_State *L, int index);
    template<>	int					read(lua_State *L, int index);
    template<>	unsigned int		read(lua_State *L, int index);
    template<>	float				read(lua_State *L, int index);
    template<>	double				read(lua_State *L, int index);
    template<>	bool				read(lua_State *L, int index);
    template<>	void				read(lua_State *L, int index);
    template<>	long long			read(lua_State *L, int index);
    template<>	unsigned long long	read(lua_State *L, int index);
    template<>	table				read(lua_State *L, int index);

    // push a value to lua stack
    //PUSH 东东给lua的，时候，一般的类型都有特化，但如果实在没有就用这个函数
    template<typename T>
    void push(lua_State *L, T ret)					{ type2lua<T>(L, ret); }

    template<>	void push(lua_State *L, char ret);
    template<>	void push(lua_State *L, unsigned char ret);
    template<>	void push(lua_State *L, short ret);
    template<>	void push(lua_State *L, unsigned short ret);
    template<>	void push(lua_State *L, long ret);
    template<>	void push(lua_State *L, unsigned long ret);
    template<>	void push(lua_State *L, int ret);
    template<>	void push(lua_State *L, unsigned int ret);
    template<>	void push(lua_State *L, float ret);
    template<>	void push(lua_State *L, double ret);
    template<>	void push(lua_State *L, char* ret);
    template<>	void push(lua_State *L, const char* ret);
    template<>	void push(lua_State *L, bool ret);
    template<>	void push(lua_State *L, lua_value* ret);
    template<>	void push(lua_State *L, long long ret);
    template<>	void push(lua_State *L, unsigned long long ret);
    template<>	void push(lua_State *L, table ret);
    //template<> 	void push(lua_State *L, std::string ret);	// 特化push函数

    // pop a value from lua stack
    template<typename T>
    T pop(lua_State *L) { T t = read<T>(L, -1); lua_pop(L, 1); return t; }

    template<>	void	pop(lua_State *L);
    template<>	table	pop(lua_State *L);

    // functor
    //LUA调用C函数的封装
    //注意functor 还有一组return void 的函数的偏特化函数
    //以这个函数为基础讲解
    //push 将结果RVal 压入堆栈
    //upvalue_ 其实是一个传统的static 函数调用类内部函数的方法，只不过upvalue_是通过LUA 特有的upvalue实现的
    //upvalue_ 最后返回的函数指针，通过函数指针完成函数调用
    //read 完成从堆栈读取参数的事情
    template<typename RVal, typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void, typename T5 = void>
    struct functor
    {
        static int invoke(lua_State *L) { push(L, upvalue_<RVal(*)(T1, T2, T3, T4, T5)>(L)(read<T1>(L, 1), read<T2>(L, 2), read<T3>(L, 3), read<T4>(L, 4), read<T5>(L, 5))); return 1; }
    };

    template<typename RVal, typename T1, typename T2, typename T3, typename T4>
    struct functor<RVal, T1, T2, T3, T4>
    {
        static int invoke(lua_State *L) { push(L, upvalue_<RVal(*)(T1, T2, T3, T4)>(L)(read<T1>(L, 1), read<T2>(L, 2), read<T3>(L, 3), read<T4>(L, 4))); return 1; }
    };

    template<typename RVal, typename T1, typename T2, typename T3>
    struct functor<RVal, T1, T2, T3>
    {
        static int invoke(lua_State *L) { push(L, upvalue_<RVal(*)(T1, T2, T3)>(L)(read<T1>(L, 1), read<T2>(L, 2), read<T3>(L, 3))); return 1; }
    };

    template<typename RVal, typename T1, typename T2>
    struct functor<RVal, T1, T2>
    {
        static int invoke(lua_State *L) { push(L, upvalue_<RVal(*)(T1, T2)>(L)(read<T1>(L, 1), read<T2>(L, 2))); return 1; }
    };

    template<typename RVal, typename T1>
    struct functor<RVal, T1>
    {
        static int invoke(lua_State *L) { push(L, upvalue_<RVal(*)(T1)>(L)(read<T1>(L, 1))); return 1; }
    };

    template<typename RVal>
    struct functor<RVal>
    {
        static int invoke(lua_State *L) { push(L, upvalue_<RVal(*)()>(L)()); return 1; }
    };

    //
    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    struct functor<void, T1, T2, T3, T4, T5>
    {
        static int invoke(lua_State *L) { upvalue_<void(*)(T1, T2, T3, T4, T5)>(L)(read<T1>(L, 1), read<T2>(L, 2), read<T3>(L, 3), read<T4>(L, 4), read<T5>(L, 5)); return 0; }
    };

    template<typename T1, typename T2, typename T3, typename T4>
    struct functor<void, T1, T2, T3, T4>
    {
        static int invoke(lua_State *L) { upvalue_<void(*)(T1, T2, T3, T4)>(L)(read<T1>(L, 1), read<T2>(L, 2), read<T3>(L, 3), read<T4>(L, 4)); return 0; }
    };

    template<typename T1, typename T2, typename T3>
    struct functor<void, T1, T2, T3>
    {
        static int invoke(lua_State *L) { upvalue_<void(*)(T1, T2, T3)>(L)(read<T1>(L, 1), read<T2>(L, 2), read<T3>(L, 3)); return 0; }
    };

    template<typename T1, typename T2>
    struct functor<void, T1, T2>
    {
        static int invoke(lua_State *L) { upvalue_<void(*)(T1, T2)>(L)(read<T1>(L, 1), read<T2>(L, 2)); return 0; }
    };

    template<typename T1>
    struct functor<void, T1>
    {
        static int invoke(lua_State *L) { upvalue_<void(*)(T1)>(L)(read<T1>(L, 1)); return 0; }
    };

    template<>
    struct functor<void>
    {
        static int invoke(lua_State *L) { upvalue_<void(*)()>(L)(); return 0; }
    };

    // push_functor
    //注释见最后一个函数
    template<typename RVal>
    void push_functor(lua_State *L, RVal(*func)())
    {
        lua_pushcclosure(L, functor<RVal>::invoke, 1);
    }

    template<typename RVal, typename T1>
    void push_functor(lua_State *L, RVal(*func)(T1))
    {
        lua_pushcclosure(L, functor<RVal, T1>::invoke, 1);
    }

    template<typename RVal, typename T1, typename T2>
    void push_functor(lua_State *L, RVal(*func)(T1, T2))
    {
        //functor<RVal,T1,T2>::invoke    C函数指针
        //1                             表示函数关联的upvalue个数   1个upvalue出栈
        lua_pushcclosure(L, functor<RVal, T1, T2>::invoke, 1);//将1个C闭包压栈
    }

    template<typename RVal, typename T1, typename T2, typename T3>
    void push_functor(lua_State *L, RVal(*func)(T1, T2, T3))
    {
        lua_pushcclosure(L, functor<RVal, T1, T2, T3>::invoke, 1);
    }

    template<typename RVal, typename T1, typename T2, typename T3, typename T4>
    void push_functor(lua_State *L, RVal(*func)(T1, T2, T3, T4))
    {
        lua_pushcclosure(L, functor<RVal, T1, T2, T3, T4>::invoke, 1);
    }

    //只解释一个函数了RVal返回值，
    //T1……T5函数的参数
    template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
    void push_functor(lua_State *L, RVal(*func)(T1, T2, T3, T4, T5))
    {
        //放入的closue 是 functor<RVal, T1, T2>::invoke 一个static 函数
        //最后的1表示有一个upvalue
        lua_pushcclosure(L, functor<RVal, T1, T2, T3, T4, T5>::invoke, 1);
    }

    // member variable
    // var_base 不是一个有模板的函数，这样就可以保证通过void * 转换为 var_base *
    // 而通过var_base *的调用 get, set 帮忙恰恰能直接调用到真正的mem_var <T,V>
    struct var_base
    {
        //原来作者这个地方写漏了一个析构处理
        //virtual ~var_base();

        virtual void get(lua_State *L) = 0;
        virtual void set(lua_State *L) = 0;
    };

    //T 为class 类型，
    //V 为变量类型
    template<typename T, typename V>
    struct mem_var : var_base
    {
        V T::*_var;
        mem_var(V T::*val) : _var(val) {}
        //get是LUA读取的操作，也就是把C++的数据读取到LUA里面，所以是PUSH
        void get(lua_State *L)
        {
            //read其实就是把类的对象的指针读取出来。
            //注意这个地方使用的是引用
            push(L, read<T*>(L, 1)->*(_var));
        }

        void set(lua_State *L)
        {
            read<T*>(L, 1)->*(_var) = read<V>(L, 3);
        }
    };

    // member function
    template<typename RVal, typename T, typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void, typename T5 = void>
    struct mem_functor
    {
        static int invoke(lua_State *L) { push(L, (read<T*>(L, 1)->*upvalue_<RVal(T::*)(T1, T2, T3, T4, T5)>(L))(read<T1>(L, 2), read<T2>(L, 3), read<T3>(L, 4), read<T4>(L, 5), read<T5>(L, 6)));; return 1; }
    };

    template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
    struct mem_functor<RVal, T, T1, T2, T3, T4>
    {
        static int invoke(lua_State *L) { push(L, (read<T*>(L, 1)->*upvalue_<RVal(T::*)(T1, T2, T3, T4)>(L))(read<T1>(L, 2), read<T2>(L, 3), read<T3>(L, 4), read<T4>(L, 5))); return 1; }
    };

    template<typename RVal, typename T, typename T1, typename T2, typename T3>
    struct mem_functor<RVal, T, T1, T2, T3>
    {
        static int invoke(lua_State *L) { push(L, (read<T*>(L, 1)->*upvalue_<RVal(T::*)(T1, T2, T3)>(L))(read<T1>(L, 2), read<T2>(L, 3), read<T3>(L, 4))); return 1; }
    };

    template<typename RVal, typename T, typename T1, typename T2>
    struct mem_functor<RVal, T, T1, T2>
    {
        static int invoke(lua_State *L) { push(L, (read<T*>(L, 1)->*upvalue_<RVal(T::*)(T1, T2)>(L))(read<T1>(L, 2), read<T2>(L, 3))); return 1; }
    };

    template<typename RVal, typename T, typename T1>
    struct mem_functor<RVal, T, T1>
    {
        static int invoke(lua_State *L) { push(L, (read<T*>(L, 1)->*upvalue_<RVal(T::*)(T1)>(L))(read<T1>(L, 2))); return 1; }
    };

    template<typename RVal, typename T>
    struct mem_functor<RVal, T>
    {
        static int invoke(lua_State *L) { push(L, (read<T*>(L, 1)->*upvalue_<RVal(T::*)()>(L))()); return 1; }
    };

    template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
    struct mem_functor<void, T, T1, T2, T3, T4, T5>
    {
        static int invoke(lua_State *L)  { (read<T*>(L, 1)->*upvalue_<void(T::*)(T1, T2, T3, T4, T5)>(L))(read<T1>(L, 2), read<T2>(L, 3), read<T3>(L, 4), read<T4>(L, 5), read<T5>(L, 6)); return 0; }
    };

    template<typename T, typename T1, typename T2, typename T3, typename T4>
    struct mem_functor<void, T, T1, T2, T3, T4>
    {
        static int invoke(lua_State *L)  { (read<T*>(L, 1)->*upvalue_<void(T::*)(T1, T2, T3, T4)>(L))(read<T1>(L, 2), read<T2>(L, 3), read<T3>(L, 4), read<T4>(L, 5)); return 0; }
    };

    template<typename T, typename T1, typename T2, typename T3>
    struct mem_functor<void, T, T1, T2, T3>
    {
        static int invoke(lua_State *L)  { (read<T*>(L, 1)->*upvalue_<void(T::*)(T1, T2, T3)>(L))(read<T1>(L, 2), read<T2>(L, 3), read<T3>(L, 4)); return 0; }
    };

    template<typename T, typename T1, typename T2>
    struct mem_functor<void, T, T1, T2>
    {
        static int invoke(lua_State *L)  { (read<T*>(L, 1)->*upvalue_<void(T::*)(T1, T2)>(L))(read<T1>(L, 2), read<T2>(L, 3)); return 0; }
    };

    template<typename T, typename T1>
    struct mem_functor<void, T, T1>
    {
        static int invoke(lua_State *L)  { (read<T*>(L, 1)->*upvalue_<void(T::*)(T1)>(L))(read<T1>(L, 2)); return 0; }
    };

    template<typename T>
    struct mem_functor<void, T>
    {
        static int invoke(lua_State *L)  { (read<T*>(L, 1)->*upvalue_<void(T::*)()>(L))(); return 0; }
    };

    // push_functor
    template<typename RVal, typename T>
    void push_functor(lua_State *L, RVal(T::*func)())
    {
        lua_pushcclosure(L, mem_functor<RVal, T>::invoke, 1);
    }

    template<typename RVal, typename T>
    void push_functor(lua_State *L, RVal(T::*func)() const)
    {
        lua_pushcclosure(L, mem_functor<RVal, T>::invoke, 1);
    }

    template<typename RVal, typename T, typename T1>
    void push_functor(lua_State *L, RVal(T::*func)(T1))
    {
        lua_pushcclosure(L, mem_functor<RVal, T, T1>::invoke, 1);
    }

    template<typename RVal, typename T, typename T1>
    void push_functor(lua_State *L, RVal(T::*func)(T1) const)
    {
        lua_pushcclosure(L, mem_functor<RVal, T, T1>::invoke, 1);
    }

    template<typename RVal, typename T, typename T1, typename T2>
    void push_functor(lua_State *L, RVal(T::*func)(T1, T2))
    {
        lua_pushcclosure(L, mem_functor<RVal, T, T1, T2>::invoke, 1);
    }

    template<typename RVal, typename T, typename T1, typename T2>
    void push_functor(lua_State *L, RVal(T::*func)(T1, T2) const)
    {
        lua_pushcclosure(L, mem_functor<RVal, T, T1, T2>::invoke, 1);
    }

    template<typename RVal, typename T, typename T1, typename T2, typename T3>
    void push_functor(lua_State *L, RVal(T::*func)(T1, T2, T3))
    {
        lua_pushcclosure(L, mem_functor<RVal, T, T1, T2, T3>::invoke, 1);
    }

    template<typename RVal, typename T, typename T1, typename T2, typename T3>
    void push_functor(lua_State *L, RVal(T::*func)(T1, T2, T3) const)
    {
        lua_pushcclosure(L, mem_functor<RVal, T, T1, T2, T3>::invoke, 1);
    }

    template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
    void push_functor(lua_State *L, RVal(T::*func)(T1, T2, T3, T4))
    {
        lua_pushcclosure(L, mem_functor<RVal, T, T1, T2, T3, T4>::invoke, 1);
    }

    template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
    void push_functor(lua_State *L, RVal(T::*func)(T1, T2, T3, T4) const)
    {
        lua_pushcclosure(L, mem_functor<RVal, T, T1, T2, T3, T4>::invoke, 1);
    }

    //注意这儿，注意这儿呀。我就写一次注释，
    //push_functor 有2种，一种是针对类内部成员（非static）函数的，
    //mem_functor是成员函数的模版
    template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
    void push_functor(lua_State *L, RVal(T::*func)(T1, T2, T3, T4, T5))
    {
        lua_pushcclosure(L, mem_functor<RVal, T, T1, T2, T3, T4, T5>::invoke, 1);
    }

    //针对成员函数函数的还有一种形式，const的，
    template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
    void push_functor(lua_State *L, RVal(T::*func)(T1, T2, T3, T4, T5) const)
    {
        lua_pushcclosure(L, mem_functor<RVal, T, T1, T2, T3, T4, T5>::invoke, 1);
    }

    //new 一个 userdata,对应class，同时完成构造函数，
    //同时设置userdata的metatable为class_add定义的那个metatable
    // constructor
    template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
    int constructor(lua_State *L)
    {
        //new 一个user data，用val2user<T>的大小,同时，同时用placement new 的方式（指针式lua_newuserdata分配的），
        //完成构造函数
        //val2user<T> 是一个从user继承的类,user是一个包装void * 的类，
        //这样的封装，让meta_get，和meta_set的调用称为了可能，
        //meta_get，meta_set其实没有模版信息，而meta_get和meta_get是在class 注册的时候使用的，
        //那时候还没有注册对象成员
        new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L, 2), read<T2>(L, 3), read<T3>(L, 4), read<T4>(L, 5), read<T5>(L, 6));
        push_meta(L, class_name<typename class_type<T>::type>::name());
        lua_setmetatable(L, -2);

        return 1;
    }

    template<typename T, typename T1, typename T2, typename T3, typename T4>
    int constructor(lua_State *L)
    {
        new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L, 2), read<T2>(L, 3), read<T3>(L, 4), read<T4>(L, 5));
        push_meta(L, class_name<typename class_type<T>::type>::name());
        lua_setmetatable(L, -2);

        return 1;
    }

    template<typename T, typename T1, typename T2, typename T3>
    int constructor(lua_State *L)
    {
        new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L, 2), read<T2>(L, 3), read<T3>(L, 4));
        push_meta(L, class_name<typename class_type<T>::type>::name());
        lua_setmetatable(L, -2);

        return 1;
    }

    template<typename T, typename T1, typename T2>
    int constructor(lua_State *L)
    {
        new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L, 2), read<T2>(L, 3));
        push_meta(L, class_name<typename class_type<T>::type>::name());
        lua_setmetatable(L, -2);

        return 1;
    }

    template<typename T, typename T1>
    int constructor(lua_State *L)
    {
        new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L, 2));
        push_meta(L, class_name<typename class_type<T>::type>::name());
        lua_setmetatable(L, -2);

        return 1;
    }

    template<typename T>
    int constructor(lua_State *L)
    {
        new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>();
        push_meta(L, class_name<typename class_type<T>::type>::name());
        lua_setmetatable(L, -2);

        return 1;
    }

    //帮助垃圾回收器调用析构函数
    //其实从这个代码上看，没有必要用模版函数<T>，他是直接用user基类依靠虚函数搞掂的。
    //整个函数没有地方用到了T，
    //调用USER_DATA的基类的析构,由于user其实是一个LUA使用的userdata对象的基类，
    //其子类包括3种，val,ptr,ref,其中val的析构会释放对象，ptr，ref的对象什么都不会做，
    //这样就保证无论你传递给LUA什么，他们的生命周期都是正确的，

    // destroyer
    template<typename T>
    int destroyer(lua_State *L)
    {
        ((user*)lua_touserdata(L, 1))->~user();
        return 0;
    }

    // table function
    template<typename F>
    void subdef(lua_State* L, const char* name, F func)
    {
        lua_pushstring(L, name);
        lua_pushlightuserdata(L, (void*)func);
        push_functor(L, func);
        lua_settable(L, 1);
    }

    // global function
    //定义一个全局函数，或者类的静态函数给lua调用
    template<typename F>
    void def(lua_State* L, const char* name, F func)
    {
        lua_pushstring(L, name);    //函数名称
        lua_pushlightuserdata(L, (void*)func);    //将函数指针转换为void * ，作为lightuserdata 放入堆栈，作为closure的upvalue放入
        push_functor(L, func);    //模板函数，放入closure,func是模板参数
        lua_settable(L, LUA_GLOBALSINDEX);    //将其放入全局环境表中
    }

    // global variable
    //全局变量的get set，
    template<typename T>
    void set(lua_State* L, const char* name, T object)
    {
        //名称对象，
        lua_pushstring(L, name);
        //模板函数，根据T绝对如何push
        push(L, object);
        lua_settable(L, LUA_GLOBALSINDEX);
    }

    template<typename T>
    T get(lua_State* L, const char* name)
    {
        lua_pushstring(L, name);
        lua_gettable(L, LUA_GLOBALSINDEX);
        return pop<T>(L);
    }

    template<typename T>
    void decl(lua_State* L, const char* name, T object)
    {
        set(L, name, object);
    }

    // call //调用LUA的函数
    template<typename RVal>
    RVal call(lua_State* L, const char* name)
    {
        lua_pushcclosure(L, on_error, 0);
        int errfunc = lua_gettop(L);

        lua_pushstring(L, name);
        lua_gettable(L, LUA_GLOBALSINDEX);
        if (lua_isfunction(L, -1))
        {
            if (lua_pcall(L, 0, 1, errfunc) != 0)
            {
                lua_pop(L, 1);
            }
        }
        else
        {
            print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
        }

        lua_remove(L, -2);
        return pop<RVal>(L);
    }

    template<typename RVal, typename T1>
    RVal call(lua_State* L, const char* name, T1 arg)
    {
        lua_pushcclosure(L, on_error, 0);
        int errfunc = lua_gettop(L);

        lua_pushstring(L, name);
        lua_gettable(L, LUA_GLOBALSINDEX);
        if (lua_isfunction(L, -1))
        {
            push(L, arg);
            if (lua_pcall(L, 1, 1, errfunc) != 0)
            {
                lua_pop(L, 1);
            }
        }
        else
        {
            print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
        }

        lua_remove(L, -2);
        return pop<RVal>(L);
    }

    //以这个为例子讲解，
    //RVal 返回值
    //T1，T2参数

    template<typename RVal, typename T1, typename T2>
    RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2)
    {
        lua_pushcclosure(L, on_error, 0);    //放入错误处理的函数，并且记录堆栈的地址
        int errfunc = lua_gettop(L);//栈的高度

        lua_pushstring(L, name);//压栈
        lua_gettable(L, LUA_GLOBALSINDEX);
        if (lua_isfunction(L, -1))//栈顶是否为函数
        {
            //放入堆栈参数，arg1，arg2，
            push(L, arg1);
            push(L, arg2);
            if (lua_pcall(L, 2, 1, errfunc) != 0)//2个参数，1个结果，errfunc是错误处理函数 在栈上的索引位置
            {
                lua_pop(L, 1);//从栈中弹出1个元素
            }
        }
        else
        {
            print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
        }
        //在堆栈删除掉错误处理的函数
        lua_remove(L, -2);//从给定有效索引-2处移除一个元素， 把这个索引之上的所有元素移下来填补上这个空隙
        //在堆栈弹出返回值
        return pop<RVal>(L);
    }

    template<typename RVal, typename T1, typename T2, typename T3>
    RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3)
    {
        lua_pushcclosure(L, on_error, 0);
        int errfunc = lua_gettop(L);

        lua_pushstring(L, name);
        lua_gettable(L, LUA_GLOBALSINDEX);
        if (lua_isfunction(L, -1))
        {
            push(L, arg1);
            push(L, arg2);
            push(L, arg3);
            if (lua_pcall(L, 3, 1, errfunc) != 0)
            {
                lua_pop(L, 1);
            }
        }
        else
        {
            print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
        }

        lua_remove(L, -2);
        return pop<RVal>(L);
    }


    // class init
    //类的初始化，让class能在lua中使用
    //定义类的metatable的表，或者说原型的表。
    template<typename T>
    void class_add(lua_State* L, const char* name)
    {
        //绑定T和名称
        class_name<T>::name(name);

        //类的名称
        lua_pushstring(L, name);
        lua_newtable(L);    //new 一个table，这个table是作为其他的类的metatable的（某种程度上也可以说是原型），

        //__name不是标准的元方法，但在例子中有使用
        lua_pushstring(L, "__name");
        lua_pushstring(L, name);
        lua_rawset(L, -3);

        //将meta_get函数作为__index函数
        lua_pushstring(L, "__index");
        lua_pushcclosure(L, meta_get, 0);
        lua_rawset(L, -3);

        //将meta_set函数作为__newindex函数
        lua_pushstring(L, "__newindex");
        lua_pushcclosure(L, meta_set, 0);
        lua_rawset(L, -3);

        //垃圾回收函数
        lua_pushstring(L, "__gc");
        lua_pushcclosure(L, destroyer<T>, 0);
        lua_rawset(L, -3);

        lua_settable(L, LUA_GLOBALSINDEX);
    }

    // Tinker Class Inheritence
    // 实现类的继承关系，T和P都要先class_add
    template<typename T, typename P>
    void class_inh(lua_State* L)
    {
        //根据子类的名称，取得子类的metatable
        push_meta(L, class_name<T>::name());
        //判断栈顶是否为表
        if (lua_istable(L, -1))
        {
            //设置__parent 为 父类名称，目前不能多重继承
            lua_pushstring(L, "__parent");
            push_meta(L, class_name<P>::name());
            lua_rawset(L, -3);
        }
        //从堆栈弹出push_meta取得的vlue
        lua_pop(L, 1);
    }

    // Tinker Class Constructor
    // T 是类
    // F 是构造函数的封装，lua_tinker::constructor
    template<typename T, typename F>
    void class_con(lua_State* L, F func)
    {
        //根据类的名称，取得类的metatable的表，或者说原型。
        push_meta(L, class_name<T>::name());
        //如果栈顶是一个表
        if (lua_istable(L, -1))
        {
            //对这个类的metatable的表，设置一个metatable，在其中增加一个__call的对应函数
            //这样的目的是这样的，__call是对应一个()调用，但实体不是函数式，的调用函数
            //LUA中出现这样的调用，
            //object =class_name()
            lua_newtable(L);
            lua_pushstring(L, "__call");
            lua_pushcclosure(L, func, 0);
            lua_rawset(L, -3);
            //设置这个table作为class 原型的metatable.
            //或者说设置这个table作为class metatable的metatable.
            lua_setmetatable(L, -2);
        }
        lua_pop(L, 1);
    }

    // Tinker Class Functions
    template<typename T, typename F>
    void class_def(lua_State* L, const char* name, F func)
    {
        push_meta(L, class_name<T>::name());
        if (lua_istable(L, -1))
        {
            lua_pushstring(L, name);
            //这个类的函数指针作为upvalue_的。
            //注意这儿是类的成员指针（更加接近size_t），而不是实际的指针，所以这儿不能用light userdata
            new(lua_newuserdata(L, sizeof(F))) F(func);
            push_functor(L, func);
            lua_rawset(L, -3);
        }
        lua_pop(L, 1);
    }

    // Tinker Class Variables
    // T 绑定的类
    // VAR 是绑定的变量的类型，
    // BASE 成员所属的类，一般我认为T和BASE是一样的
    template<typename T, typename BASE, typename VAR>
    void class_mem(lua_State* L, const char* name, VAR BASE::*val)
    {
        //根据类的名称，取得类的metatable
        push_meta(L, class_name<T>::name());
        if (lua_istable(L, -1))
        {
            lua_pushstring(L, name);
            //mem_var 继承于var_base,实际调用的时候利用var_base的虚函数完成回调。
            new(lua_newuserdata(L, sizeof(mem_var<BASE, VAR>))) mem_var<BASE, VAR>(val);
            lua_rawset(L, -3);
        }
        lua_pop(L, 1);
    }

    //用模板函数辅助帮忙实现一个方法，可以通过class 找到对应的类名称（注册到LUA的名称），
    template<typename T>
    struct class_name
    {
        // global name
        static const char* name(const char* name = NULL)
        {
            static char temp[256] = "";
            if (name) strcpy(temp, name);
            return temp;
        }
    };

    // Table Object on Stack
    struct table_obj
    {
        table_obj(lua_State* L, int index);
        ~table_obj();

        void inc_ref();
        void dec_ref();

        bool validate();

        template<typename T>
        void set(const char* name, T object)
        {
            if (validate())
            {
                lua_pushstring(m_L, name);
                push(m_L, object);
                lua_settable(m_L, m_index);
            }
        }

        template<typename T>
        T get(const char* name)
        {
            if (validate())
            {
                lua_pushstring(m_L, name);
                lua_gettable(m_L, m_index);
            }
            else
            {
                lua_pushnil(m_L);
            }

            return pop<T>(m_L);
        }

        lua_State*		m_L;
        int				m_index;
        const void*		m_pointer;
        int				m_ref;
    };

    // Table Object Holder
    struct table
    {
        table(lua_State* L);
        table(lua_State* L, int index);
        table(lua_State* L, const char* name);
        table(const table& input);
        ~table();

        template<typename T>
        void set(const char* name, T object)
        {
            m_obj->set(name, object);
        }

        template<typename T>
        T get(const char* name)
        {
            return m_obj->get<T>(name);
        }

        table_obj*		m_obj;
    };

} // namespace lua_tinker

#endif //_LUA_TINKER_H_
