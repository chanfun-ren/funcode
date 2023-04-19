## basic key-value sys

1. 通过命令行测试。

   - 三个子命令，手写实现。✅
   - 也可利用第三方命令行解析工具 cxxopt, cmdline.

   > the interface for the CLI is:
   >
   > - `ckv set <KEY> <VALUE>`
   >
   >   Set the value of a string key to a string
   >
   > - `ckv get <KEY>`
   >
   >   Get the string value of a given string key
   >
   > - `ckv rm <KEY>`
   >
   >   Remove a given key
   >
   > - `ckv -V`
   >
   >   Print the version

2. 错误处理设计。✅

   - 参考 leveldb 错误码

3. in-memory db 测试 ✅

4. 读写过程（WAL + in-memory index）✅

   > bitcask

   - 实现 set，get，rm 过程中日志的操作。

   - set 流程：

     - 将操作（LogEntry）写入日志。
     - 确保 flush 到磁盘后，写/更新内存中的哈希表/索引，存储着 key 对应的文件的 offset。
     - 返回结果

   - rm 流程：

     - 先判断所删除的 key 是否存在

     - 如果存在，则将操作写入日志（和 set 操作统一，不过写入的是一条空记录）

       > 注意：虽然 `set` 和 `rm` 统一，如果删除某 key，in-memory index 也会删除 key。

     - 返回结果

   - get 流程：

     暂时不修改 in-memory index 中的 log 指针。KvStore 启动时重放 log，读取 log 中的数据，填充 in-memory index.

     - 先读 in-memory index
     - 如果存在 key，根据读到的 offset 读取 log_entry
     - 返回结果

5. 初始化过程 ✅

   - 创建打开数据库文件
   - 重放日志

6. `in-memory index + WAL` db 测试. ✅

   - 比较测试日志文件内容

7. 文件管理(TODO)


## + network

### CLI

1. ckv-client

   - implements the functionality required for ckv-client to speak to ckv-server
   - 类似之前的 ./ckv, 不过加入了一个额外选项 [--addr IP-PORT]
   
   > the interface for the CLI is:
   >
   > - `ckv-client set <KEY> <VALUE> [--addr IP-PORT]`
   >
   >   Set the value of a string key to a string
   >
   > - `ckv-client get <KEY> [--addr IP-PORT]`
   >
   >   Get the string value of a given string key
   >
   > - `ckv-client rm <KEY> [--addr IP-PORT]`
   >
   >   Remove a given key
   >
   > - `ckv-client -V [--addr IP-PORT]`
   >
   >   Print the version

   - 实现 KvClient, KvClient 负责建立与 KvServer 的连接和通信 ✅
   - client_main 通过 KvClient 执行命令 ✅
   - 定义 client 请求 Cmd, 收到的回复 Response 消息格式, 实现其序列化反序列函数并测试. ✅
   > "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n"

2. ckv-server

  - implements the functionality to serve responses to ckv-client from ckv-server
  - 实现 KvServer, 接收 KvClient 请求，解析 Command, 执行对应操作，将结果返回给 KvClient. ✅
  - 搭建网络框架(epoll)，处理客户端请求 ✅
  