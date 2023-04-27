#include <iostream>
#include <vector>

using std::vector;

// mem: 33, 81 和 nums[0] 比较 => 得出有用的非递减序列，缩小范围。
// mem: 153 本题也可与 nums[0] 比较
class Solution {
 public:
  int findMin(vector<int>& nums) {
    if (nums.empty()) return -1;
    int left = 0;
    int right = nums.size() - 1;
    if (nums[0] < nums[right]) {  // special case: 旋转 n 次，刚好是全部升序
      return nums[0];
    }
    while (left < right) {
      int mid = left + (right - left) / 2;
      if (nums[mid] > nums[0]) {
        left = mid + 1;
      } else if (nums[mid] < nums[0]) {
        if (mid > 0 && nums[mid - 1] > nums[mid]) {  // mid 为旋转点
          return nums[mid];
        } else {
          right = mid - 1;
        }
      } else if (nums[mid] == nums[0]) {
        left = mid + 1;
      }
    }
    return nums[left];
  }
};

int main() {
  vector<int> v = {11, 13, 15, 17};
  Solution s;
  std::cout << s.findMin(v) << std::endl;

  return 0;
}