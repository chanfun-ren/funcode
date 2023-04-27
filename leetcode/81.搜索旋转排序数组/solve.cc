#include <iostream>
#include <vector>

using std::vector;

// mem: 33, 81 和 nums[0] 比较 => 得出有用的非递减序列，缩小范围。
class Solution {
 public:
  bool search(vector<int>& nums, int target) {
    if (nums.empty()) return false;
    int left = 0;
    int right = nums.size() - 1;
    while (left <= right) {
      int mid = left + (right - left) / 2;
      if (nums[mid] == target) {
        return true;
      }
      if (nums[mid] > nums[0]) {
        if (target >= nums[left] && target < nums[mid]) {
          right = mid - 1;
        } else {
          left = mid + 1;
        }
      } else if (nums[mid] == nums[0]) {  // 无法判断
        if (nums[left] == target) return true;
        left = left + 1;
      } else if (nums[mid] < nums[0]) {
        if (target > nums[mid] && target <= nums[right]) {
          left = mid + 1;
        } else {
          right = mid - 1;
        }
      }
    }
    return false;
  }
};

int main() {
  vector<int> v = {2, 5, 6, 0, 0, 1, 2};
  Solution s;
  std::cout << s.search(v, 3) << std::endl;

  return 0;
}