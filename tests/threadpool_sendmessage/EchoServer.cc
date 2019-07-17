#include "muduo/net/TcpServer.h"

#include "muduo/base/Logging.h"
#include "muduo/base/Thread.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"

#include "muduo/base/ThreadPool.h"
#include "muduo/base/Timestamp.h"

#include <utility>

#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

class EchoServer
{
public:
  EchoServer(EventLoop *loop, const InetAddress &listenAddr, int buffersize, int totalmessages)
      : loop_(loop),
        server_(loop, listenAddr, "EchoServer"),
        message_(new char[buffersize]),
        totalmessages_(totalmessages)
  {
    server_.setConnectionCallback(
        std::bind(&EchoServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&EchoServer::onMessage, this, _1, _2, _3));
    server_.setThreadNum(0);

    memset(message_, 'd', buffersize);
  }

  void start()
  {
    server_.start();
  }
  // void stop();

private:
  void onConnection(const TcpConnectionPtr &conn)
  {
    // LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
    //           << conn->localAddress().toIpPort() << " is "
    //           << (conn->connected() ? "UP" : "DOWN");
    // LOG_INFO << conn->getTcpInfoString();

    // conn->send("hello\n");

    ThreadPool pool;
    pool.setMaxQueueSize(16);
    pool.start(16);

    Timestamp begin = Timestamp::now();
    for (int i = 0; i < totalmessages_; i++)
    {
      pool.run(std::bind(&EchoServer::sendMessage, this, conn, message_));
    }

    CountDownLatch latch(1);
    pool.run(std::bind(&CountDownLatch::countDown, &latch));
    latch.wait();
    Timestamp end = Timestamp::now();
    printf("send over %d messages cost %.2f second.\n", totalmessages_, timeDifference(end, begin));

    pool.stop();
  }

  void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time)
  {
    // string msg(buf->retrieveAllAsString());
    // printf("server recived: %s\n", msg.c_str());
  }

  void sendMessage(const TcpConnectionPtr &conn, const char *message)
  {
    conn->send(message);
  }

  EventLoop *loop_;
  TcpServer server_;
  char *message_;     // 发送buffer
  int totalmessages_; //发送的次数
};

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Usage: %s <buffer size> <totoal messsages>\n", argv[0]);
    return -1;
  }

  LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
  LOG_INFO << "sizeof TcpConnection = " << sizeof(TcpConnection);

  int buffersize = atoi(argv[1]);
  int totalmessages = atoi(argv[2]);

  EventLoop loop;
  InetAddress listenAddr(9888, false);
  EchoServer server(&loop, listenAddr, buffersize, totalmessages);

  server.start();

  loop.loop();
}
