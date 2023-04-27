#include <iostream>
#include <vector>

using std::vector;

class Solution {
 public:
  // using int = int;
  vector<int> sortArray(vector<int>& nums) {
    // 快速排序：分治 =>
    // 0. partiton
    // 1. quicksort(half)
    // 2. auto sorted
    sort(nums, 0, nums.size() - 1);
    return nums;
  }
  // `sort()` sort [begin, end]
  void sort(vector<int>& v, int begin, int end) {
    if (begin >= end) return;
    int it = partition(v, begin, end);
    // std::cout << "--:" << it << "\n";
    sort(v, begin, it - 1);
    sort(v, it + 1, end);
  }
  int partition(vector<int>& nums, int left, int right) {
    int pivot = nums[left];
    int i = left, j = right;
    while (i < j) {
      while (i < j && nums[j] >= pivot) j--;
      nums[i] = nums[j];
      while (i < j && nums[i] <= pivot) i++;
      nums[j] = nums[i];
    }
    nums[i] = pivot;
    return i;
  }
};

template <typename T>
void print_vec(const vector<T>& v) {
  for (const auto& e : v) {
    std::cout << e << " ";
  }
  std::cout << std::endl;
}

int main() {
  Solution s;
  vector v = {4, 2, 3, 1};
  print_vec(v);
  auto res = s.sortArray(v);
  print_vec(res);

  return 0;
}