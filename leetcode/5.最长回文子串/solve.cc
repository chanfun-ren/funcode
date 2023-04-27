#include <iostream>
#include <vector>

using std::string;
using std::vector;

template <typename T>
void print_vec(const vector<T>& v) {
  for (const auto& e : v) {
    std::cout << e << " ";
  }
  std::cout << std::endl;
}

class Solution {
 public:
  string longestPalindrome(string& s) {
    if (s.empty()) return "";
    int left = 0;
    int right = 0;
    int max_len = 1;
    int n = s.size();
    vector<vector<int>> dp(n, vector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
      dp[i][i] = 1;
    }
    auto more_and_update = [&left, &right, &max_len, &dp](int i, int j) {
      if (dp[i][j] > max_len) {
        max_len = dp[i][j];
        left = i;
        right = j;
      }
    };
    for (int j = 0; j < n; ++j) {
      for (int i = j - 1; i >= 0; --i) {
        if (i == j - 1) {
          dp[i][j] = (s[i] == s[j] ? 2 : 0);
          more_and_update(i, j);
          continue;
        }
        if (s[i] == s[j] && dp[i + 1][j - 1] != 0) {
          dp[i][j] = dp[i + 1][j - 1] + 2;
          more_and_update(i, j);
        } else {
          dp[i][j] = 0;
        }
      }
    }
    std::cout << left << ", " << max_len << "\n";
    for (auto v : dp) {
      print_vec(v);
    }
    return s.substr(left, max_len);
  }
};

int main() {
  string str = "cbbd";
  Solution s;
  std::cout << s.longestPalindrome(str) << std::endl;
  return 0;
}