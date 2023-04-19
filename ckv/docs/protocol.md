# protocol

> 目前只支持字符串类型的 SET, GET, RM 操作

## 通信协议

0. 请求 Command:
   
   ```cpp
   enum CommandType { SET, GET, RM, INVALID };
   struct Command {
    CommandType type;
    std::string key;
    std::string value;
   };
   ```

Redis协议使用"*"和"\$"表示参数的数量和参数的长度。对于SET命令，协议中应包含3个参数，分别为"SET"、键 和 值，每个参数的格式为"$长度\r\n参数值
\r\n"。对于GET和RM命令，协议中应包含2个参数，分别为"GET"/"RM"和键，每个参数的格式同上。

例如：`set key value` 会被序列化为 `*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n`

1. 回复 Response:

  ```cpp
  enum ResponseType { STATUS, ERROR, UNKNOWN, DATA };
  struct Response {
    ResponseType type;
    std::string message;
  }
  ```

  改编Redis协议，使用"+"、"-"和"?"、"\$"表示响应的类型，分别对应 ResponseType::STATUS、ResponseType::ERROR 和 ResponseType::UNKNOWN。
  响应消息的格式为"+/-/?/$message\r\n"。