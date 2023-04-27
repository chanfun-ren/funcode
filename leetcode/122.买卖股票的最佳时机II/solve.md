```cpp
- dp: dp[i][0], dp[i][1] 分别表示第 i 天不持有股票，持有股票的最大利润
- res: dp[n-1][0]，最后一天肯定手上没股票利润最大
- dp[i][0] = max(dp[i-1][0], dp[i-1][1] + prices[i])
- dp[i][1] = max(dp[i-1][1], dp[i-1][0] - prices[i])
```