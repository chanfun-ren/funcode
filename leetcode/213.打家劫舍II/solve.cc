#include <iostream>
#include <vector>

using std::vector;

class Solution {
 public:
  int rob(vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }
    auto rob = [&nums](int begin, int end) {  // [begin, end)
      int prev_do_steal = nums[begin];
      int prev_dont = 0;
      for (size_t i = begin + 1; i < end; ++i) {
        int tmp = prev_do_steal;
        prev_do_steal = prev_dont + nums[i];
        prev_dont = std::max(prev_dont, tmp);
      }
      return std::max(prev_do_steal, prev_dont);
    };
    int n = nums.size();
    if (n == 1) return nums[0];
    return std::max(rob(1, n), rob(0, n - 1));
  }
};

int main() {
  vector<int> v = {1};
  Solution s;
  std::cout << s.rob(v) << std::endl;

  return 0;
}