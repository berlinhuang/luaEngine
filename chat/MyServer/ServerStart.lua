--
-- Created by IntelliJ IDEA.
-- User: root
-- Date: 18-1-17
-- Time: 下午7:07
-- To change this template use File | Settings | File Templates.
--
local cppFunc, startServer = cppFunc, startServer
local print = print
module(...)

print("test plus func with 2 params cpp_func(1,2) = "..cppFunc(1,2))


startServer("127.0.0.1",9981)