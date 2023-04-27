#include <iostream>
#include <vector>

using std::vector;

// mem: 33, 81 和 nums[0] 比较 => 得出有用的非递减序列，缩小范围。
// mem: 153 也可与 nums[0] 比较
// mem: 154 本题也可与 nums[0] 比较，nums[mid] == nums[0]
// 时，无法缩小大片范围，right -= 1;
class Solution {
 public:
  int findMin(vector<int>& nums) {
    if (nums.empty()) return -1;
    int n = nums.size();
    int left = 0;
    int right = n - 1;
    if (nums[0] < nums[n - 1]) {  // special case: 旋转 n 次，刚好是全部升序
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
        if (right > 0 && nums[right - 1] > nums[right]) {  // right 为旋转点
          return nums[right];
        } else {
          right -= 1;
        }
      }
    }
    return nums[left];
  }
};

int main() {
  vector<int> v = {3, 3, 1, 3};
  Solution s;
  std::cout << s.findMin(v) << std::endl;

  return 0;
}