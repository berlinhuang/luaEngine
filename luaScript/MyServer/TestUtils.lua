--
-- Created by IntelliJ IDEA.
-- User: root
-- Date: 18-2-1
-- Time: 下午9:11
-- To change this template use File | Settings | File Templates.
--


local print, require, table, _G = print, require, table, _G
local CFServer, CFSys = CFServer, CFSys
local InetAddress, StringArg, Thread, EventLoop = InetAddress, StringArg, Thread, EventLoop
module(...)
local Utils = require("Utils")
table.print = Utils.printTable
table.deepCopy = Utils.deepCopy


local myTable = {
    firstName = "Fred",
    lastName = "Bob",
    phoneNumber = "(555) 555-1212",
    age = 30,
    favoriteSports = { "Baseball", "Hockey", "Soccer" },
    favoriteTeams  = { "Cowboys", "Panthers", "Reds" },
}


function testExport()
    table.print(CFServer)
    table.print(CFSys)
end


function testUtils()
--    table.print(myTable)
    local myTableCopy = table.deepCopy( myTable )
    myTableCopy.sex = "male"
    table.print(myTable)
    table.print(myTableCopy)
end

function threadFunc()
    print("in threading...")
end


function printObj()
    local strAddr = StringArg("192.168.50.135")
    local inetAddr = InetAddress(strAddr,12)
    Utils.objinfo(inetAddr)
    local loop = EventLoop()
    Utils.objinfo(loop)

--    local threadFunc = Function( threadFunc);
--    local thread = Thread( threadFunc,"mythread")
--    Utils.objinfo(Thread)
end

function printG()
    table.print(_G)
end

--testExport()
--testUtils()

printObj()
