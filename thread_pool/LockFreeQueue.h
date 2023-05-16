#pragma once

#include <atomic>
#include <memory>

template <typename T>
class LockFreeQueue {
 private:
  struct Node {
    T data;
    std::atomic<Node*> next;
    Node(const T& data_) : data(data_) { next.load(nullptr); }
  };

  // TODO: 改为 shared_ptr<Node>.
  std::atomic<Node*> head;
  std::atomic<Node*> tail;
  // size_t std::atomic<size_t> size;

 public:
  LockFreeQueue() {
    head.load(nullptr);
    tail.load(nullptr);
  }

  // bool empty() { return head.load() == nullptr; }
  // size_t size() { return size.load(); }

  void push(const T& data) {
    std::atomic<Node*> const new_node = new Node(data);
    Node* old_tail = tail.load();
    while (!old_tail->next.compare_exchange_weak(nullptr, new_node)) {
      /* 尝试使用 compare_exchange_weak 方法将原子指针 old_tail->next 的值从
       * nullptr
       * 替换为 new_node，如果替换成功，则说明新节点已经成功插入到了队列尾部，
       * 退出循环。如果替换失败，则可能是因为有其他线程同时在插入节点，需要重新加载
       * tail 指针并重试。
       */
      old_tail = tail.load();
    }

    // 使用 compare_exchange_weak 方法将原子指针 tail 的值从 old_tail
    // 替换为 new_node。如果替换失败，则说明其他线程已经更新了 tail 指针.
    tail.compare_exchange_weak(old_tail, new_node);
  }

  void pop() {
    // TODO: free(old_head)
    Node* old_head = head.load();
    while (old_head && !head.compare_exchange_weak(old_head, old_head->next)) {
      // 尝试将 head 的值从 old_head 替换为 old_head->next.
      // 如果更新成功，即 head 已经指向了后一个元素（pop 成功）
      // 如果更新失败，说明有其他线程在删除节点，重新加载 head 重试
      old_head = head.load();
    }
    // return old_head ? old_head->data : T();
  }

  T front() { return head.load(); }
};