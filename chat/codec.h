#ifndef MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H
#define MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H

#include <muduo/base/Logging.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Endian.h>
#include <muduo/net/TcpConnection.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>

typedef muduo::string Message;
class MessageQueue :
                      public boost::enable_shared_from_this<MessageQueue>
{
public:
  MessageQueue(const muduo::net::TcpConnectionPtr& conn, const char* buf, 
                  const int32_t len, const muduo::Timestamp timestamp) : 
    conn_(conn),
    message_(buf, len),
    timestamp_(timestamp)
  {}
  muduo::net::TcpConnectionPtr conn_;
  Message message_;
  muduo::Timestamp timestamp_;
};
typedef boost::shared_ptr<MessageQueue> MessageQueuePtr;


class LengthHeaderCodec : boost::noncopyable
{
 public:
  typedef boost::function<void (const muduo::net::TcpConnectionPtr&,
                                const muduo::string& message,
                                muduo::Timestamp)> StringMessageCallback;
  typedef boost::function<void (const MessageQueuePtr&)> QueueMessageCallback;

  explicit LengthHeaderCodec(const QueueMessageCallback& cb)
    : messageQueueCallback_(cb)
  {
  }

  void onMessage(const muduo::net::TcpConnectionPtr& conn,
                 muduo::net::Buffer* buf,
                 muduo::Timestamp receiveTime)
  {
    while (buf->readableBytes() >= kHeaderLen) // kHeaderLen == 4
    {
      // FIXME: use Buffer::peekInt32()
      // const void* data = buf->peek();
      // int32_t be32 = *static_cast<const int32_t*>(data); // SIGBUS
      // const int32_t len = muduo::net::sockets::networkToHost32(be32);
      const int32_t len = buf->peekInt32();
      if (len > 65536 || len < 0)
      {
        LOG_ERROR << "Invalid length " << len;
        conn->shutdown();  // FIXME: disable reading
        break;
      }
      else if (buf->readableBytes() >= len + kHeaderLen)
      {
        buf->retrieve(kHeaderLen);
        // 按消息头里面的度, 读取数据, 并把它转为对应的结构
        // muduo::string message(buf->peek(), len);
        // messageCallback_(conn, message, receiveTime);
        MessageQueuePtr messageQueue = 
          boost::make_shared<MessageQueue>(conn, buf->peek(), len, receiveTime);
        messageQueueCallback_(messageQueue);
        buf->retrieve(len);
      }
      else
      {
        break;
      }
    }
  }

  // FIXME: TcpConnectionPtr
  void send(muduo::net::TcpConnection* conn,
            const muduo::StringPiece& message)
  {
    muduo::net::Buffer buf;
    buf.append(message.data(), message.size());
    int32_t len = static_cast<int32_t>(message.size());
    int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
    buf.prepend(&be32, sizeof be32);
    conn->send(&buf);
  }

 private:
  QueueMessageCallback messageQueueCallback_;
  const static size_t kHeaderLen = sizeof(int32_t);
};

#endif  // MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H
