#include <iostream>
#include <vector>
using std::vector;

class Solution {
 public:
  int maxSubArray(vector<int>& nums) {
    int prev = -1;
    int res = INT_MIN;
    for (size_t i = 0; i < nums.size(); ++i) {
      int curr = prev < 0 ? nums[i] : prev + nums[i];
      res = std::max(curr, res);
      prev = curr;
    }
    return res;
  }
};

int main() {
  Solution s;
  vector v = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
  vector v1 = {5, 4, -1, 7, 8};
  std::cout << s.maxSubArray(v1) << std::endl;
  return 0;
}