#include <iostream>
#include <vector>

using std::vector;

class Solution {
 public:
  int maxProfit(vector<int>& prices) {
    int n = prices.size();
    if (n == 0) return 0;
    int initial = 0;
    int buy1 = -1 * prices[0];
    int sell1 = 0;
    int buy2 = -1 * prices[0];
    int sell2 = 0;
    for (int i = 1; i < n; ++i) {
      buy1 = std::max(initial - prices[i], buy1);
      sell1 = std::max(sell1, buy1 + prices[i]);
      buy2 = std::max(buy2, sell1 - prices[i]);
      sell2 = std::max(sell2, buy2 + prices[i]);
    }
    return sell2;
  }
};

int main() {
  vector<int> v = {7, 6, 4, 3, 1};
  vector<int> v1 = {1, 2, 3, 4, 5};
  Solution s;
  std::cout << s.maxProfit(v) << std::endl;

  return 0;
}