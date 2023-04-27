```cpp
- 4 个步骤，最终产生 5 种状态，定义状态下的最大利润
- initial, buy1, sell1, buy2, sell2
- buy1 = max(initial - prices[i], buy1)
- sell1 = max(sell1, buy1 + prices[i])
- buy2 = max(buy2, sell1 - prices[i])
- sell2 = max(sell2, buy2 + prices[i])
- 边界条件 i = 0: buy1 = -prices[0], sell1 = 0; buy2 = 买卖买 = -prices[0], sell2 = 买卖买卖 = 0
```