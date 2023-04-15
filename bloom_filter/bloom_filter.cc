#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

using HashArrayType = std::vector<std::function<size_t(int)>>;
HashArrayType default_hash_function = {
    [](int e) -> size_t { return std::abs(e) * 1; },
    [](int e) -> size_t { return std::abs(e) * 2; },
    [](int e) -> size_t { return std::abs(e) * 3; }};

// TODO: 暂时只兼容 int 型元素
class BloomFilter {
 private:
  // m bits
  uint64_t bit_map_size_;

  // bitmap: stores the elements' hash values
  std::vector<int> bit_map_;

  // number of hash functions. (k)
  uint8_t hash_func_num_;

  // k hash functions, set by user
  std::vector<std::function<size_t(int)>> hash_functions_;

 public:
  BloomFilter(int m) : bit_map_size_(m) {
    bit_map_.resize(bit_map_size_, 0);
    hash_functions_ = default_hash_function;
  }

  // 一次性设置多个哈希函数
  void SetHashFunction(std::vector<std::function<size_t(int)>>& functions) {
    hash_functions_ = functions;
    hash_func_num_ = hash_functions_.size();
  }

  // 供用户添加哈希函数
  void AddHashFunction(std::function<size_t(int)> func) {
    hash_functions_.push_back(std::move(func));
    hash_func_num_ = hash_functions_.size();
  }

  // 清除所有哈希函数
  void ClearHashFunctions() {
    hash_functions_ = {};
    hash_func_num_ = 0;
  }

  // Contains(ele): false positives may happen.
  bool Contains(int ele) {
    // 计算该元素的 k 个哈希值，检测它们在位图中是否为置位
    for (size_t i = 0; i < hash_functions_.size(); ++i) {
      size_t hash_val = hash_functions_[i](ele);
      if (!bit_map_[hash_val % bit_map_size_]) {
        return false;
      }
    }
    return true;
  }

  void Insert(int ele) {
    // 计算该元素的 k 个哈希值，并在位图中是否为置位
    for (size_t i = 0; i < hash_functions_.size(); ++i) {
      size_t hash_val = hash_functions_[i](ele);
      bit_map_[hash_val % bit_map_size_] = true;
    }
  }

  // bloom filter does not support `delete` operation.

  // ---- Funcions for Test -----
 public:
  void PrintBitMap() {
    std::cout << "index: ";
    for (size_t i = 0; i < bit_map_.size(); i++) {
      if (bit_map_[i]) {
        std::cout << i << ", ";
      }
    }
    std::cout << " is set.\n";
  }
};

int main() {
  // 位图大小 1024, 使用默认哈希函数
  BloomFilter bl(1024);
  bl.Insert(34);
  bl.PrintBitMap();
  assert(bl.Contains(34));

  bl.ClearHashFunctions();
  bl.AddHashFunction([](int e) { return e * 4; });

  bl.Insert(12);
  bl.PrintBitMap();
  assert(!bl.Contains(34));  // 额外添加了一个哈希函数
  assert(bl.Contains(12));
  assert(!bl.Contains(11));

  return 0;
}