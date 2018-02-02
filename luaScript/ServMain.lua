--接收程序运行参数
local strServType = string.match( g_progArgs , "-s%s+(%w+)" )
if strServType == nil then
    print( "usage: -s [MyServer|SceneServer|DBServer|]" )
    CFSys.exit(-1)
end

if strServType == 'MyServer' then
    package.path = "./MyServer/?.lua;./Common/?/Init.lua;./MyServer/?.lua;./MyServer/?/Init.lua" --
end
--
require("ServStart")



