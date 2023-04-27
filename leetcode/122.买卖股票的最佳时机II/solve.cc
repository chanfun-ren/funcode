#include <iostream>
#include <vector>

using std::vector;

class Solution {
 public:
  int maxProfit(vector<int>& prices) {
    int n = prices.size();
    if (n == 0) return 0;
    // vector<vector<int>> dp(n, vector<int>(n, 0));
    // dp[0][0] = 0;
    // dp[0][1] = -1 * prices[0];
    int profit0 = 0;
    int profit1 = -1 * prices[0];
    for (int i = 1; i < n; ++i) {
      profit0 = std::max(profit0, profit1 + prices[i]);
      profit1 = std::max(profit1, profit0 - prices[i]);
      // dp[i][0] = std::max(dp[i - 1][0], dp[i - 1][1] + prices[i]);
      // dp[i][1] = std::max(dp[i - 1][1], dp[i - 1][0] - prices[i]);
    }
    // return dp[n - 1][0];
    return profit0;
  }
};

int main() {
  vector<int> v = {1};
  Solution s;
  std::cout << s.maxProfit(v) << std::endl;

  return 0;
}