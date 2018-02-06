--
-- Created by IntelliJ IDEA.
-- User: root
-- Date: 18-1-17
-- Time: 下午7:07
-- To change this template use File | Settings | File Templates.
--

local   CFServer = CFServer
local   print, require =  print, require
module(...)

function onNewConnection( ip )
    print(ip)
end

function onMessage( tm, msg )
    print(tm,msg);
end


function startServer()
    CFServer._setNewConnectionCallback(onNewConnection)
    CFServer._setMessageCallback(onMessage)
    CFServer._startServer("127.0.0.1",9981)
end

--startServer()


require( "TestUtils")

