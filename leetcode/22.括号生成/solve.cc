#include <iostream>
#include <vector>

using std::string;
using std::vector;

class Solution {
 public:
  vector<string> solutions;
  vector<string> generateParenthesis(int n) {
    string res;
    dfs(n, res, 0, 0);
    return solutions;
  }

  void dfs(int n, string& res, int left, int right) {
    auto invalid = [=]() { return left < right || left > n || right > n; };
    if (invalid()) {
      return;
    }
    if (res.size() == 2 * n) {
      solutions.push_back(res);
      return;
    }
    res.push_back('(');
    dfs(n, res, left + 1, right);
    res.pop_back();

    res.push_back(')');
    dfs(n, res, left, right + 1);
    res.pop_back();
  }
};

template <typename T>
void print_vec(const vector<T>& v) {
  for (const auto& e : v) {
    std::cout << e << "\n";
  }
  std::cout << std::endl;
}

int main() {
  int n = 2;
  Solution s;
  auto res = s.generateParenthesis(n);
  print_vec(res);

  return 0;
}