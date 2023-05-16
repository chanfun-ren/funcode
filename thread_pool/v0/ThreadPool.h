#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  ThreadPool(size_t);
  template <class F, class... Args>
  auto enqueue(F&& f, Args&&... args)
      -> std::future<typename std::invoke_result_t<F, Args...>>;
  ~ThreadPool();

 private:
  // need to keep track of threads so we can join them
  std::vector<std::thread> workers;

  // the task queue
  std::queue<std::function<void()>> tasks;

  // synchronization
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads) : stop(false) {
  for (size_t i = 0; i < threads; ++i)
    workers.emplace_back([this] {
      for (;;) {
        std::function<void()> task;

        {
          std::unique_lock<std::mutex> lock(this->queue_mutex);
          // ThreadPool 终止运行 或者 任务队列非空 => stop waiting, 即被唤醒。
          // 换言之，如果 ThreadPool 是
          // active状态且任务队列仍有待运行任务，worker
          // 就应该不断取任务，执行任务
          this->condition.wait(
              lock, [this] { return this->stop || !this->tasks.empty(); });

          if (this->stop && this->tasks.empty()) {
            // ThreadPool 终止运行 => `stop` 为 true
            // 并且任务队列中没有待运行的任务，线程函数返回，即线程终止运行
            return;
          }
          // 不断从队列中获取任务
          task = std::move(this->tasks.front());
          this->tasks.pop();
        }
        // 执行任务
        task();
      }
    });
}

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result_t<F, Args...>> {
  // -> std::future<typename std::result_of<F(Args...)>::type> {
  // using return_type = typename std::result_of<F(Args...)>::type;
  using return_type = std::invoke_result_t<F, Args...>;

  // 将传入的任务函数封装成 packaged_task 类型的 `task`
  // packaged_task 将 f 与一个 std::future
  // 关联起来，使得 f 可以异步执行，并且可以通过
  // std::future 获取它的返回值或异常。
  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(queue_mutex);

    // don't allow enqueueing after stopping the pool
    if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");

    // 用 lambda 封装成类似 std::function<void()> 的 callable 对象
    // 加入等待队列
    tasks.emplace([task]() { (*task)(); });
  }
  // 加入任务后，通知/唤醒在队列上等待的 worker 线程
  condition.notify_one();

  // 返回 future<return_type>, 保存未来的执行结果
  return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  // ThreadPool 销毁 => 告知所有 worker
  // 线程，执行完等待队列中剩余任务就退出线程函数（终止运行）。
  condition.notify_all();

  // join 所有 worker thread
  for (std::thread& worker : workers) worker.join();
}

#endif