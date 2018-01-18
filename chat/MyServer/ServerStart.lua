--
-- Created by IntelliJ IDEA.
-- User: root
-- Date: 18-1-17
-- Time: 下午7:07
-- To change this template use File | Settings | File Templates.
--

local   CFServer, CTTest, CFNet =
        CFServer, CTTest, CFNet
local   print, pairs, type, require, table =
        print, pairs, type, require, table
local   _G = _G
module(...)


local Utils = require("Utils")
table.print = Utils.print_r


local mytb = {
    firstName = "Fred",
    lastName = "Bob",
    phoneNumber = "(555) 555-1212",
    age = 30,
    favoriteSports = { "Baseball", "Hockey", "Soccer" },
    favoriteTeams  = { "Cowboys", "Panthers", "Reds" },
}

--table.print(_G)
--table.print(mytb)
--table.print(CFServer)
--table.print(CTTest)
--
function onMessage( a, b )
    print(a,b);
end


function startServer()
    CFNet._setMessageCallback(onMessage)

    CFServer.startServer("127.0.0.1",9981)
end

startServer()

--print("test plus func with 2 params cpp_func(1,2) = "..CFServer.cppFunc(1,2))

