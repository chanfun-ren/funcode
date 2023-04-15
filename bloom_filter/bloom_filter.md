# Bloom Filter

## 原理

- m 位大小的位图 + k 个哈希函数 

- insert: 计算元素的 k 个哈希值，设置位图的 k 位为 1

- search: 计算 k 个哈希值，判断那 k 位是否全被置位

- 无法进行 delete 操作

  > 因为保存的并不是实际元素，而是它们的哈希值信息，如果允许删除可能会删除其他元素。

- 同样：可能会出现假阳性的误判。False Postives.

## False Postives 分析

直观上：m 越大，k 越大，n 越小，误报率越低。

**假阳性含义**：某元素 e 不在集合内部，但是 Bloom Filter 认为在。即 e 的 k 位全被置 1 了。

- 插入一个元素，某 bit 不被 set 的概率是 (1 - 1/m)^k^

  > 可能会有疑问：哈希函数不一样，计算出的 k 个位置必定不一样，一位被 set 的概率是 (1/m)，下一位被 set 的概率应该是 (1/(m-1)). 这里只是粗略估计。

- 插入 n 个元素，某 bit 不被 set 的概率是 (1 - 1/m)^nk^,  被 set 的概率是 1 - (1 - 1/m)^nk^

- 那么误报的概率（k 位被 set 了）是，(1 - (1 - 1/m)^nk^)^k^.  m 很大时，近似 (1 - e^-kn/m^)^k^

- 假设 m 受程序运行环境限制，n 根据应用场景可估量，应该**合理选择 k 使得误报率尽量低**。

函数表达式：(1 - e^-kn/m^)^k^

​	*k*=ln(2)⋅*m*/*n*.  上式最小。

## 复杂度分析

空间: O(m)

时间：insert, search 均为 O(k).

## 代码

- `bloom_filter.cc`/ `bloom_filter_com.cc`



## Ref

- [Bloom Filter \| Brilliant Math & Science Wiki](https://brilliant.org/wiki/bloom-filter/#:~:text=A%20bloom%20filter%20is%20a,is%20added%20to%20the%20set.)







