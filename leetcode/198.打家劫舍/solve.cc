#include <iostream>
#include <vector>

using std::vector;

class Solution {
 public:
  int rob(vector<int>& nums) {
    if (nums.empty()) return 0;
    int prev_do_steal = nums[0];
    int prev_dont = 0;
    for (size_t i = 1; i < nums.size(); ++i) {
      int tmp = prev_do_steal;
      prev_do_steal = prev_dont + nums[i];
      prev_dont = std::max(tmp, prev_dont);
    }
    return std::max(prev_do_steal, prev_dont);
  }
};

int main() {
  vector<int> v = {2, 7, 9, 3, 1};
  Solution s;
  std::cout << s.rob(v) << std::endl;

  return 0;
}