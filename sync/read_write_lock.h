#include <condition_variable>
#include <mutex>

// 0.偏向读者：临界区有其他读者时，另一个读者和一个写者同时申请进入临界区，读者可以进入
// 1.偏向写者：临界区有其他读者时，有新的写者到达时阻塞后续读者
class ReadWriteLock {
 private:
  std::condition_variable cv_;

  std::mutex counter_mutex_;  // protect reader_count;
  int reader_couter_ = 0;

  std::mutex writer_mutex_;

 public:
  void RLock() {
    counter_mutex_.lock();
    reader_couter_++;
    if (reader_couter_ == 1) {
      // first reader => lock writer_mutex
      writer_mutex_.lock();
    }
    counter_mutex_.unlock();
  }

  void RUnlock() {
    counter_mutex_.lock();
    reader_couter_--;
    if (reader_couter_ == 0) {
      writer_mutex_.unlock();
    }
    counter_mutex_.unlock();
  }
  void WLock() { writer_mutex_.lock(); }
  void WUnlock() { writer_mutex_.unlock(); }
};