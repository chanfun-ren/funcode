
#include <iostream>
#include <vector>
using std::vector;

class Solution {
 public:
  // 找出某元素的右边最大值，做差结果为该天买入能够获益最大值
  // => 从右 scan
  int maxProfit(vector<int>& prices) {
    int res = 0;
    int right_max = 0;
    for (auto it = prices.rbegin(); it != prices.rend(); ++it) {
      right_max = std::max(right_max, *it);
      res = std::max(res, right_max - *it);
    }
    return res;
  }
};

int main() {
  vector input = {7, 1, 5, 3, 6, 4};

  Solution s;
  std::cout << s.maxProfit(input) << "\n";

  vector input1 = {4, 2, 0, 3, 2, 5};
  std::cout << s.maxProfit(input1) << "\n";

  return 0;
}