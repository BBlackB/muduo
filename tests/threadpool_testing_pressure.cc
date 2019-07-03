#include "muduo/base/ThreadPool.h"
#include "muduo/base/CountDownLatch.h"
#include "muduo/base/Timestamp.h"

#include <stdio.h>
#include <ctime>

#define s_rand() double(1.0 * rand() / RAND_MAX)

double MontePI(int n)
{
  double PI;
  double x, y;
  int i = 0, sum = 0;
  srand(static_cast<unsigned>(time(NULL)));
  for (i = 0; i < n; i++)
  {
    x = s_rand();
    y = s_rand();
    if (x * x + y * y < 1)
    {
      sum++;
    }
  }
  PI = 4.0 * sum / n;
  return PI;
}

void print(int n)
{
  // double pi = MontePI(100000);
  // printf("pi = %lf\n", pi);
  // (void)pi;

  // printf("hello.\n");
}

int main(int argc, char const *argv[])
{

  muduo::ThreadPool pool("ThreadPool");

  pool.setMaxQueueSize(100);
  pool.start(2);

  muduo::Timestamp begin(muduo::Timestamp::now());
  int n = 1000000;
  while (n--)
  {
    pool.run(std::bind(print, n));
  }

  muduo::CountDownLatch latch(1);
  pool.run(std ::bind(&muduo::CountDownLatch::countDown, &latch));
  latch.wait();
  pool.stop();

  muduo::Timestamp end(muduo::Timestamp::now());
  printf("total time:%f\n", muduo::timeDifference(end, begin));

  return 0;
}
