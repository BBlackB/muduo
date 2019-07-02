#include "muduo/base/ThreadPool.h"
#include "muduo/base/CountDownLatch.h"
#include "muduo/base/Timestamp.h"

#include <stdio.h>
#include <ctime>

// #define s_rand() double(1.0 * rand() / RAND_MAX)

// double MontePI(int n)
// {
//   double PI;
//   double x, y;
//   int i = 0, sum = 0;
//   srand((int)time(0));
//   for (i = 0; i < n; i++)
//   {
//     x = s_rand();
//     y = s_rand();
//     if (x * x + y * y < 1)
//     {
//       sum++;
//     }
//   }
//   PI = 4.0 * sum / n;
//   return PI;
// }

// double arctan(double x)
// {

//   double sqr = x * x;

//   double e = x;

//   double r = 0;

//   int i = 1;

//   while (e / i > 1e-15)
//   {

//     double f = e / i;

//     r = (i % 4 == 1) ? r + f : r - f;

//     e = e * sqr;

//     i += 2;
//   }

//   return r;
// }

void print()
{
  // int n = 10;
  // while (n--)
  // {
  //   double a = 16.0 * arctan(1 / 5.0);
  //   double b = 4.0 * arctan(1 / 239.0);
  //   double c = a - b;
  //   c--;
  // }
  // double pi = MontePI(5000);
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

  // printf("done.\n");

  muduo::CountDownLatch latch(1);
  pool.run(std ::bind(&muduo::CountDownLatch::countDown, &latch));
  latch.wait();
  pool.stop();

  muduo::Timestamp end(muduo::Timestamp::now());
  printf("total time:%f\n", muduo::timeDifference(end, begin));

  return 0;
}
