

#include <iostream>
#include <vector>
using std::max, std::min;
using std::vector;

class Solution {
 public:
  int trap(vector<int>& height) {
    if (height.empty()) {
      return 0;
    }

    size_t n = height.size();
    vector<int> left_max_hight(n, 0);
    left_max_hight[0] = height[0];
    vector<int> right_max_hight(n, 0);
    right_max_hight[n - 1] = height[n - 1];

    for (size_t i = 1; i < n; ++i) {
      left_max_hight[i] = max(left_max_hight[i - 1], height[i]);
    }
    for (int i = n - 2; i >= 0; --i) {  // be careful: `size_t` type
      right_max_hight[i] = max(right_max_hight[i + 1], height[i]);
    }

    int res = 0;
    for (size_t i = 0; i < n; ++i) {
      // 当前小格能够存储的雨水数: 依赖于 两边最高值的较小那个 - 当前高度
      res += min(left_max_hight[i], right_max_hight[i]) - height[i];
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