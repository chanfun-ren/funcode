<!-- language: lang-cn -->

# ckv

## 简介

- 一个简单的键值存储系统（目前支持字符串类型的 set, get, rm 操作）
- 主要思想基于 [bitcask](https://riak.com/assets/bitcask-intro.pdf) 实现。ckv 的存储引擎主要由两个部分组成：内存索引（index）和磁盘日志（WAL）。所有的写操作都追加到一个日志文件中，保证顺序 IO；当日志文件大小超过一定阈值（`COMPACTION_THRESHOLD`）时，会对日志文件进行压缩，避免日志文件无限增长。系统启动时重放日志，构建内存索引。索引保存每个键的位置信息（即在日志文件中的偏移量和长度）。
- 提供 CLI 工具 `ckv-cli`，通过网络与 `ckv-server` 建立通信，通信协议参考 RESP 协议自定义实现。
- server 端使用 epoll 并发处理客户端请求。

## 构建

> ckv 依赖于 C++17 标准库和支持 epoll 的平台，构建过程需要使用 CMake 工具。

1. mkdir build && cd build
2. cmake ..
3. make

## 使用

### 服务器端

ckv 的服务器端程序通过命令行参数设置监听端口和数据库存储路径。

使用方式：

```sh
./ckv-server [bind_port] [db_path]
```

其中 `bind_port` 和 `db_path` 是可选参数。

- `bind_port` 是服务器监听的端口号，默认为 8888。
- `db_path` 是数据库的存储路径，默认为 "./db"。



### 客户端

ckv 提供了一个命令行界面（CLI），客户端可以通过该界面进行键值操作。

使用方式：

```sh
./ckv-cli [server_addr]
```

其中 `server_addr` 是可选参数，表示服务器的地址，默认为 "127.0.0.1:8888"。

CLI 的接口为：

- `kvs set <KEY> <VALUE>`：将一个字符串键的值设置为一个字符串。
- `kvs get <KEY>`：获取给定字符串键的字符串值。
- `kvs rm <KEY>`：删除给定键。

### 示例

```shell
➜  build git:(master) ✗ ./ckv-server 
Database initialized successfully
```

```shell
➜  build git:(master) ✗ ./ckv-cli 
connected to sever.
> set key value
OK
> get key
value
> set name 西门吹雪
OK
> get name
西门吹雪
> rm name
OK
> get name
Key not found
> set name 菠萝吹雪
OK
> get name
菠萝吹雪
```

[English](../README.md)

