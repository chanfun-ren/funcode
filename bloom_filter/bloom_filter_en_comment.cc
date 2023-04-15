#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

using HashArrayType = std::vector<std::function<size_t(int)>>;
HashArrayType default_hash_function = {
    [](int e) -> size_t { return std::abs(e) * 1; },
    [](int e) -> size_t { return std::abs(e) * 2; },
    [](int e) -> size_t { return std::abs(e) * 3; }};

// TODO: Only support `int` type elements for now.
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

  // Set multiple hash functions at once
  void SetHashFunction(std::vector<std::function<size_t(int)>>& functions) {
    hash_functions_ = functions;
    hash_func_num_ = hash_functions_.size();
  }

  // Add a hash function for user
  void AddHashFunction(std::function<size_t(int)> func) {
    hash_functions_.push_back(std::move(func));
    hash_func_num_ = hash_functions_.size();
  }

  // Clear all hash functions
  void ClearHashFunctions() {
    hash_functions_ = {};
    hash_func_num_ = 0;
  }

  // Contains(ele): false positives may happen.
  bool Contains(int ele) {
    // Calculate the k hash values for the element, and check if they are set
    for (size_t i = 0; i < hash_functions_.size(); ++i) {
      size_t hash_val = hash_functions_[i](ele);
      if (!bit_map_[hash_val % bit_map_size_]) {
        return false;
      }
    }
    return true;
  }

  void Insert(int ele) {
    // Calculate the k hash values for the element, and set them in the bitmap
    for (size_t i = 0; i < hash_functions_.size(); ++i) {
      size_t hash_val = hash_functions_[i](ele);
      bit_map_[hash_val % bit_map_size_] = true;
    }
  }

  // Bloom filter does not support delete operation.

  /***** Functions for Test *****/
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
  // Set the bitmap size to 1024, and use the default hash functions
  BloomFilter bl(1024);
  bl.Insert(34);
  bl.PrintBitMap();
  assert(bl.Contains(34));

  bl.ClearHashFunctions();
  bl.AddHashFunction([](int e) { return e * 4; });

  bl.Insert(12);
  bl.PrintBitMap();
  assert(!bl.Contains(34));  // Add an extra hash function
  assert(bl.Contains(12));
  assert(!bl.Contains(11));
  return 0;
}