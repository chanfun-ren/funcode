#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class MutexQueue {
 private:
  std::queue<T> queue_;
  std::mutex mutex_;

  // 条件变量 cv_ 通知正在等待的线程有新的元素可供消费
  std::condition_variable cv_;

 public:
  void push(const T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(value);
    cv_.notify_one();
  }

  bool try_pop(T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      return false;
    }
    value = queue_.front();
    queue_.pop();
    return true;
  }

  void wait_and_pop(T& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !queue_.empty(); });
    value = queue_.front();
    queue_.pop();
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

  T front() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.front();
  }
};