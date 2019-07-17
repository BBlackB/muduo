#include "muduo/net/TcpClient.h"

#include "muduo/base/Logging.h"
#include "muduo/base/Thread.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"

#include <utility>

#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

int numThreads = 0;
class EchoClient;
std::vector<std::unique_ptr<EchoClient>> clients;
int current = 0;

class EchoClient : noncopyable
{
public:
  EchoClient(EventLoop *loop, const InetAddress &listenAddr, const string &id, const int buffersize)
      : loop_(loop),
        client_(loop, listenAddr, "EchoClient" + id),
        buffersize_(buffersize),
        totalmessages_(0)
  {
    client_.setConnectionCallback(
        std::bind(&EchoClient::onConnection, this, _1));
    client_.setMessageCallback(
        std::bind(&EchoClient::onMessage, this, _1, _2, _3));
    //client_.enableRetry();
  }

  void connect()
  {
    client_.connect();
  }
  // void stop();

private:
  void onConnection(const TcpConnectionPtr &conn)
  {
    if (conn->connected())
    {
      ++current;
      if (implicit_cast<size_t>(current) < clients.size())
      {
        clients[current]->connect();
      }
      LOG_INFO << "*** connected " << current;
    }

  }

  void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time)
  {
    string msg(buf->retrieveAllAsString());
    totalmessages_ += strlen(msg.c_str());
    printf("client total recived: %ld\n", totalmessages_ / buffersize_);
    fflush(NULL);
  }

  EventLoop *loop_;
  TcpClient client_;
  int buffersize_;         // 收到的buffer大小,用于统计收到了多少个包
  uint64_t totalmessages_; // 统计收到了多少个包
};

int main(int argc, char *argv[])
{
  LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
  if (argc > 1)
  {
    EventLoop loop;
    InetAddress serverAddr(argv[1], 9888);

    int n = 1;
    if (argc != 3)
    {
      printf("Usage: %s <buffersize>\n", argv[0]);
      return -1;
    }
    int buffersize = atoi(argv[2]);

    for (int i = 0; i < n; ++i)
    {
      char buf[32];
      snprintf(buf, sizeof buf, "%d", i + 1);
      clients.emplace_back(new EchoClient(&loop, serverAddr, buf, buffersize));
    }

    clients[current]->connect();
    loop.loop();
  }
  else
  {
    printf("Usage: %s host_ip [current#]\n", argv[0]);
  }
}