//
// Created by root on 18-1-17.
//

#ifndef LUAENGINE_LUASERVER_H_H
#define LUAENGINE_LUASERVER_H_H

#include "Codec.h"

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/TcpServer.h>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/functional.hpp>
#include <list>
#include <set>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

using namespace muduo;
using namespace muduo::net;

class LuaServer : boost::noncopyable
{
public:
    LuaServer(EventLoop* loop, const InetAddress& listenAddr)
            : server_(loop, listenAddr, "LuaServer"),
              codec_(boost::bind(&LuaServer::onMessageQueue, this, _1))
    {
        server_.setConnectionCallback( boost::bind(&LuaServer::onConnection, this, _1));
        server_.setMessageCallback( boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
    }

    void start()
    {
        server_.start();
        LOG_INFO << "Server start success";
    }

    bool hasMessge(){ return !messageQueue_.empty(); }
    void distillMessageQueue(std::list<MessageQueuePtr>& messageQueue)
    {
        //TODO:
        {
            MutexLockGuard lock(mutex_);
            messageQueue.swap(messageQueue_);
        }
    }

    // when timer event occur, create and push_front message to queue
    void onTimerMessage(const MessageQueuePtr& message)
    {
        {
            MutexLockGuard lock(mutex_);
            messageQueue_.push_front(message);
        }
    }

    void onConnection(const TcpConnectionPtr& conn);

    static void hanleMessageQueue();

private:
    void onMessageQueue(const MessageQueuePtr& message)
    {
        //TODO: push to every conn's queue
        {
            MutexLockGuard lock(mutex_);
            messageQueue_.push_back(message);
        }
    }
    void onStringMessage(const TcpConnectionPtr&, const string& message, Timestamp)
    {
        for (ConnectionList::iterator it = connections_.begin();  it != connections_.end(); ++it)
        {
            codec_.send(get_pointer(*it), message);
        }
    }

    typedef std::set<TcpConnectionPtr> ConnectionList;
    TcpServer server_;
    LengthHeaderCodec codec_;
    ConnectionList connections_;

    mutable MutexLock mutex_;
    // TODO: use weak_ptr
    std::list<MessageQueuePtr> messageQueue_;
};


#endif //LUAENGINE_SERVER_H_H