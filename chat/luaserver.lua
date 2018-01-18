--接收程序运行参数
local strServType = string.match( g_progArgs , "-s%s+(%w+)" )
if strServType == nil then
    print( "usage: -s [MyServer|SceneServer|DBServer|]" )
    CFSys.exit(-1)
end

if strServType == 'MyServer' then
    package.path = "./MyServer/?.lua;./Common/?/init.lua;./MyServer/Script/?.lua;./MyServer/Script/?/init.lua" --
end
--
require("ServerStart")














-- 把C++中定义的类打入lua环境供Lua代码使用
-- 在C++中为Lua环境添加全局变量（类实例对象）
--
--
--print(g_test._test) -- 打印g_test的成员变量_test
--print(g_test:is_test())
--print(g_test:ret_int())
--
--
--
--temp = test(4) -- 创建一个test类
--print(temp._test) -- 打印test的_test成员
--
--a = g_test:get()
--
--temp:set(a)
--print(temp._test)
--print(temp:is_base())
--print(temp:is_test())
--
--
---- 对象的基本信息
---------------------------------------------------------------------------------
--function objinfo(obj)
--    local meta = getmetatable(obj)
--    if meta ~= nil then
--        metainfo(meta)
--    else
--        print("no object infomation !!")
--    end
--end
--
--function metainfo(meta)
--    if meta ~= nil then
--        local name = meta["__name"]
--        if name ~= nil then
--            metainfo(meta["__parent"])
--            print("<"..name..">")
--            -- 遍历对象的各个属性
--            for key,value in pairs(meta) do
--                if not string.find(key, "__..") then
--                    if type(value) == "function" then
--                        print("\t[f] "..name..":"..key.."()") -- [f] test:set()
--                    elseif type(value) == "userdata" then
--                        print("\t[v] "..name..":"..key) -- [v] test:_test
--                    end
--                end
--            end
--        end
--    end
--end
---------------------------------------------------------------------------------
--
---- Lua 打印对象的地址？？
--print("g_test	-> ", g_test)
--print("temp	-> ", temp)
--print("a	-> ", a)
--
---- C++ 注册的全局对象g_test
--print("objinfo(g_test)")
--objinfo(g_test)
--
---- Lua创建的对象
--print("objinfo(temp)")
--objinfo(temp)
--
---- 未注册的对象
--print("objinfo(a)")
--objinfo(a)
--



