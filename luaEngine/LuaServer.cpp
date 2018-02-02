//
// Created by root on 18-1-17.
//

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include "lua_bind.h"
#include "LuaServer.h"
using namespace std;

boost::function<void(const char*)> _onNewConnection;
boost::function<void(const char*, const char*)> _onMessage;

LuaServer *g_server = 0;
bool bHandleMsg = true;
typedef std::list<MessageQueuePtr>::iterator MessageQueueIter;


void LuaServer::onConnection(const TcpConnectionPtr& conn)
{
//        LOG_INFO << conn->localAddress().toIpPort() << " -> "
//                 << conn->peerAddress().toIpPort() << " is "
//                 << (conn->connected() ? "UP" : "DOWN");
    if (conn->connected())
    {
        connections_.insert(conn);
        _onNewConnection(conn->peerAddress().toIpPort().c_str());
    }
    else
    {
        connections_.erase(conn);
    }
}

void LuaServer::hanleMessageQueue()
{
    while (bHandleMsg)
    {
        if (g_server && g_server->hasMessge())
        {
            std::list<MessageQueuePtr> messageQueue;
            g_server->distillMessageQueue(messageQueue);
            for (MessageQueueIter itr = messageQueue.begin(); itr != messageQueue.end(); ++itr)
            {
                const char *time = (*itr)->timestamp_.toString().c_str();
                const char *msg = (*itr)->message_.c_str();
                _onMessage(time,msg);
            }
        }
    }
}


