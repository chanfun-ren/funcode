#include <algorithm>
#include <iostream>
#include <vector>

using std::vector;

class Solution {
 public:
  vector<vector<int>> threeSum(vector<int>& nums) {
    // 固定一个数。排序 => 双指针移动，去重
    if (nums.size() < 3) return {};
    vector<vector<int>> res;
    std::sort(nums.begin(), nums.end());
    size_t n = nums.size();
    for (size_t i = 0; i + 2 < nums.size(); ++i) {
      if (i > 0 && nums[i] == nums[i - 1]) {
        continue;
      }
      // vector<int> tmp;
      int target = -1 * nums[i];
      int j = i + 1;
      int k = n - 1;
      while (j < k) {
        if (nums[j] + nums[k] < target) {
          ++j;
          while (j < k && nums[j] == nums[j - 1]) {
            ++j;
          }
        } else if (nums[j] + nums[k] == target) {
          res.push_back({nums[i], nums[j], nums[k]});
          ++j;
          while (j < k && nums[j] == nums[j - 1]) {
            ++j;
          }
          --k;
          while (j < k && nums[k] == nums[k + 1]) {
            --k;
          }
        } else if (nums[j] + nums[k] > target) {
          --k;
          while (j < k && nums[k] == nums[k + 1]) {
            --k;
          }
        }
      }
    }
    return res;
  }
};

template <typename T>
void print_vec(const vector<T>& v) {
  for (const auto& e : v) {
    std::cout << e << " ";
  }
  std::cout << std::endl;
}

int main() {
  Solution s;
  vector v = {-1, 0, 1, 2, -1, -4};
  vector v1 = {0, 0, 0};
  auto res = s.threeSum(v1);
  for (auto v : res) {
    print_vec(v);
  }

  return 0;
}