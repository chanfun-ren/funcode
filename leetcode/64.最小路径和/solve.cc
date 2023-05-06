#include <climits>
#include <iostream>
#include <vector>

using std::vector;

class Solution {
 public:
  int minPathSum(vector<vector<int>>& grid) {
    int m = grid.size();
    int n = grid[0].size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, INT_MAX));
    dp[1][1] = grid[0][0];
    for (int i = 1; i <= m; ++i) {
      for (int j = 1; j <= n; ++j) {
        if (i == 1 && j == 1) {
          continue;
        }
        dp[i][j] = std::min(dp[i - 1][j], dp[i][j - 1]) + grid[i - 1][j - 1];
      }
    }
    return dp[m][n];
  }
};

int main() {
  // vector<vector<int>> v = {{1, 3, 1}, {1, 5, 1}, {4, 2, 1}};
  vector<vector<int>> v = {{1, 2, 3}, {4, 5, 6}};
  Solution s;
  std::cout << s.minPathSum(v) << std::endl;

  return 0;
}