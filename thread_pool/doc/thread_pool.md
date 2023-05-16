## 线程池简单实现（无锁任务队列 + thread_pool）

### 无锁队列

- lock-free 与使用 mutex 关键区别点在于是否有线程会被阻塞。

  > lock-free: 多线程并发访问共享资源时不使用 lock

- wait-free 和 lock-free 关键区别点：wait-free 中某线程的执行不依赖于另一个线程，或者说不需要等待某个线程完成某些操作后才可继续执行。`wait-free`模型中的某个线程仍可能阻塞，但它仍然可以在有限的时间内完成操作，而不会一直等待其他线程的操作完成。

  > A non-blocking algorithm is `lock-free` if there is guaranteed **system-wide progress**, and `wait-free` if there is also guaranteed **per-thread progress**.

  > wait-free: 多线程并发访问共享资源时，不仅不使用锁，而且每个线程的操作都能在有限的时间内完成，而不会被其他线程干扰

- 无锁 => atomic + CAS => memory_order.

- ref:

  - [Lock-free vs. wait-free concurrency - RethinkDB](https://rethinkdb.com/blog/lock-free-vs-wait-free-concurrency/)
  - [java \- Examples/Illustration of Wait\-free And Lock\-free Algorithms \- Stack Overflow](https://stackoverflow.com/questions/4211180/examples-illustration-of-wait-free-and-lock-free-algorithms)
  - [Non\-blocking algorithm \- Wikipedia](https://en.wikipedia.org/wiki/Non-blocking_algorithm#Wait-freedom)

- lock-free queue impl:

  - [Lock Free Queues. How to make a lock free queue in C++ | by Josh Segal | Medium](https://jbseg.medium.com/lock-free-queues-e48de693654b)
  - [Chapter 21. Boost.Lockfree - 1.66.0](https://www.boost.org/doc/libs/1_66_0/doc/html/lockfree.html#lockfree.introduction___motivation.introduction__amp__terminology)
  - [Implementing Lock-Free Queues (psu.edu)](https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.53.8674&rep=rep1&type=pdf)

> ```cpp
> bool compare_exchange_weak(T& expected, T desired, std::memory_order order = std::memory_order_seq_cst) noexcept;
> ```
>
> ```cpp
> bool compare_exchange_weak(T& expected, T desired){
> if this == expected:
>  this = desired;
>  return true
> return false
> }
> ```

- 测试（有点复杂？）