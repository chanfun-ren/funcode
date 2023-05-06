#include <iostream>
#include <vector>

using std::vector;

class Solution {
 public:
  bool canJump(vector<int>& nums) {
    // dp[i] 表示从当前 i 位置能跳到的最远位置  => dp[i] = max(dp[i-1], i +
    // nums[i]) dp[0] = 0 + nums[0];
    // 能否跳到最后一个位置 -> 能够再前进(dp[i] > i ?)
    if (nums.empty() || nums.size() == 1) return true;
    int n = nums.size();
    vector<int> dp(n, 0);
    dp[0] = nums[0];
    if (dp[0] == 0) return false;
    for (int i = 1; i < n - 1; ++i) {
      dp[i] = std::max(dp[i - 1], i + nums[i]);
      if (dp[i] <= i) {
        return false;
      }
    }
    return true;
  }
};

int main() {
  vector<int> v = {3, 2, 1, 0, 4};
  Solution s;
  std::cout << s.canJump(v) << std::endl;

  return 0;
}