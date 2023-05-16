#include <iostream>
#include <thread>

#include "MutexQueue.h"

void producer(MutexQueue<int>& queue) {
  for (int i = 0; i < 100000; ++i) {
    queue.push(i);
  }
}

void consumer(MutexQueue<int>& queue) {
  long long int sum = 0;
  for (int i = 0; i < 100000; ++i) {
    int x;
    queue.wait_and_pop(x);
    sum += x;
  }
  std::cout << "sum: " << sum << std::endl;
}

int main() {
  MutexQueue<int> queue;

  std::thread t1(producer, std::ref(queue));
  std::thread t2(consumer, std::ref(queue));

  long long int sum = 0;
  for (int i = 0; i < 100000; ++i) {
    sum += i;
  }
  std::cout << sum << "\n";

  t1.join();
  t2.join();

  return 0;
}