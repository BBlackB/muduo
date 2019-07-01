#include "muduo/base/ThreadPool.h"
#include "muduo/base/CountDownLatch.h"
#include "muduo/base/Timestamp.h"

#include <stdio.h>

void print()
{
  // int n = 20000;
  // while (n-- > 0)
  //   ;
  // usleep(1);
}

int main(int argc, char const *argv[])
{

  muduo::ThreadPool pool("ThreadPool");

  pool.setMaxQueueSize(100);
  pool.start(8);

  muduo::Timestamp begin(muduo::Timestamp::now());
  int n = 1000000;
  while (n--)
  {
    pool.run(print);
  }

  printf("done.\n");

  muduo::CountDownLatch latch(1);
  pool.run(std::bind(&muduo::CountDownLatch::countDown, &latch));
  latch.wait();
  pool.stop();

  muduo::Timestamp end(muduo::Timestamp::now());
  printf("total time:%f\n", muduo::timeDifference(end, begin));

  return 0;
}
