# sync tools
- some sync tools implemented in user level.(for learning purpose)


## mutex

## read-write lock

- basic idea: only one writer(allow multi readers) when accessing the shared data at the same time.
- reader: try to get a read lock. 成功申请到，继续。否则，阻塞 
- writer: try to get a write lock. 成功申请到，继续。否则，阻塞 

- 思路：
  - 如何判断是否能申请到？
  - 释放锁要做哪些工作？
  - => mutex