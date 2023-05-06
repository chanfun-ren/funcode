#include <iostream>
#include <vector>

using std::vector;

class Solution {
 public:
  int climbStairs(int n) {
    if (n <= 2) return n;
    int first = 1;
    int second = 1;
    int third = first + second;
    for (int i = 0; i < n - 2; ++i) {
      first = second;
      second = third;
      third = first + second;
    }
    return third;
  }
};

int main() {
  Solution s;
  std::cout << s.climbStairs(5) << std::endl;

  return 0;
}