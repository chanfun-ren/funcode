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

    for (size_t i = 0; i < n; i++) {
      auto [begin, end] = extend(s, i, i);
      auto [begin1, end1] = extend(s, i, i + 1);
      if (end - begin + 1 > right - left + 1) {
        right = end;
        left = begin;
      }
      if (end1 - begin1 + 1 > right - left + 1) {
        right = end1;
        left = begin1;
      }
    }
    return std::string{s.begin() + left, s.begin() + right + 1};
  }

  // 返回以 [...left, right...] 的最长回文串的起始和终止位置
  std::pair<int, int> extend(const string& s, int left, int right) {
    while (left >= 0 && right < s.size() && s[left] == s[right]) {
      --left;
      ++right;
    }
    return {left + 1, right - 1};
  }
};

int main() {
  string str = "cbbd";
  Solution s;
  std::cout << s.longestPalindrome(str) << std::endl;
  return 0;
}