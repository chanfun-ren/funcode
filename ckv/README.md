<!-- language: lang-en -->
[English](./README.md) | [中文](./docs/README-cn.md)


# ckv

## Introduction

- ckv is a simple key-value storage system that currently supports string-based set, get, and rm operations.
- The main idea behind ckv is based on [bitcask](https://riak.com/assets/bitcask-intro.pdf). The storage engine of ckv mainly consists of two parts: an in-memory index and a disk-based write-ahead log (WAL). All write operations are appended to a log file to ensure sequential IO. When the log file size exceeds a certain threshold (`COMPACTION_THRESHOLD`), it is compressed to avoid unlimited growth. During system startup, the log is replayed, and the in-memory index is constructed. The index stores the position information (i.e., offset and length in the log file) of each key.
- ckv provides a CLI tool `ckv-cli`, which communicates with `ckv-server` over the network using a custom implementation of the RESP protocol.
- The server-side of ckv uses epoll to handle client requests concurrently.

## Build

> ckv depends on the C++17 standard library and a platform that supports epoll. The build process requires the CMake tool.

1. mkdir build && cd build
2. cmake ..
3. make

## Usage

### Server-side

The ckv server program is configured to listen on a specific port and store the database using command-line arguments.

Usage:

```sh
./ckv-server [bind_port] [db_path]
```

where `bind_port` and `db_path` are optional arguments.

- `bind_port` is the port number the server listens on, defaulting to 8888.
- `db_path` is the path to store the database, defaulting to "./db".

### Client-side

ckv provides a command-line interface (CLI), which clients can use to perform key-value operations.

Usage:

```sh
./ckv-cli [server_addr]
```

where `server_addr` is optional and represents the server's address, defaulting to "127.0.0.1:8888".

The CLI interface includes:

- `kvs set <KEY> <VALUE>`: sets the value of a string key to a string.
- `kvs get <KEY>`: retrieves the string value of a given string key.
- `kvs rm <KEY>`: deletes a given key.

### Example

```shell
➜  build git:(master) ✗ ./ckv-server 
Database initialized successfully
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
