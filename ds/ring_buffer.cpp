#pragma once
#include <cstddef>
#include <stdexcept>
#include <vector>

template <typename T>
class RingBuffer {
 public:
  RingBuffer(int capacity)
      : capacity_(capacity), head_(0), tail_(0), count_(0), buffer_(capacity) {}

  ~RingBuffer() = default;

  bool empty() const { return count_ == 0; }
  bool full() const { return count_ == capacity_; }
  size_t size() const { return count_; }

  const T& front() const {
    if (empty()) {
      throw std::out_of_range("Ring buffer is empty");
    }
    return buffer_[head_];
  }

  void push_back(const T& item) {
    if (full()) {
      throw std::out_of_range("Ring buffer is full");
    }
    buffer_[tail_] = item;
    tail_ = (tail_ + 1) % capacity_;
    count_++;
  }

  void pop_front() {
    if (empty()) {
      throw std::out_of_range("Ring buffer is empty");
    }
    head_ = (head_ + 1) % capacity_;
    --count_;
  }

 private:
  size_t capacity_;
  int head_;
  int tail_;
  size_t count_;
  std::vector<T> buffer_;
};