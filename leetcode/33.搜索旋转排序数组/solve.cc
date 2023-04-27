#include <iostream>
#include <vector>

using std::vector;

class Solution {
 public:
  int search(vector<int>& nums, int target) {
    if (nums.empty()) return -1;
    int left = 0;
    int right = nums.size() - 1;
    while (left <= right) {
      int mid = left + (right - left) / 2;
      if (nums[mid] == target) {
        return mid;
      }
      if (nums[mid] < nums[0]) {   // 右边有序
        if (target < nums[mid]) {  // 一定在左边
          right = mid - 1;
        } else {  // 无法判断
          if (nums[right] == target) {
            return right;
          }
          right = right - 1;
        }
      } else {                     // 左边有序
        if (target < nums[mid]) {  // 无法判断
          if (nums[right] == target) {
            return right;
          }
          right = right - 1;
        } else {
          left = mid + 1;
        }
      }
    }
    return -1;
  }
};

int main() {
  vector<int> v = {4, 5, 6, 7, 0, 1, 2};
  Solution s;
  std::cout << s.search(v, 7) << std::endl;

  return 0;
}