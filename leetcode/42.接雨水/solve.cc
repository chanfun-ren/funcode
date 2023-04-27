

#include <iostream>
#include <vector>
using std::vector;

class Solution {
 public:
  int trap(vector<int>& height) {
    size_t n = height.size();
    if (n == 0) {
      return 0;
    }
    int res = 0;
    int left_max_h = 0, right_max_h = 0;
    int l = 0, r = n - 1;
    while (l <= r) {
      if (height[l] < height[r]) {  // l 位置能达到的高度不取决于 r
        if (height[l] >= left_max_h) {  // 这个小区间装不了雨水
          left_max_h = height[l];
          res += 0;
        } else {  // 计算可以装的数量
          res += left_max_h - height[l];
        }
        l++;
      } else {
        if (height[r] >= right_max_h) {  // 这个小区间装不了雨水
          right_max_h = height[r];
          res += 0;
        } else {  // 计算可以装的数量
          res += right_max_h - height[r];
        }
        r--;
      }
    }
    return res;
  }
};

int main() {
  vector input = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};

  Solution s;
  std::cout << s.trap(input) << "\n";

  vector input1 = {4, 2, 0, 3, 2, 5};
  std::cout << s.trap(input1) << "\n";

  return 0;
}