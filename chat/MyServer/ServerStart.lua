--
-- Created by IntelliJ IDEA.
-- User: root
-- Date: 18-1-17
-- Time: 下午7:07
-- To change this template use File | Settings | File Templates.
--
--local cppFunc, startServer = cppFunc, startServer

local CFServer = CFServer
local print, pairs = print, pairs
--local _G = _G
module(...)

--for k,v in pairs(_G) do
--    print(k,v)
--end

for name, func in pairs( CFServer) do
    print(name, "\t",func)
end


print("test plus func with 2 params cpp_func(1,2) = "..CFServer.cppFunc(1,2))

CFServer.startServer("127.0.0.1",9981)