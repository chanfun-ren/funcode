#pragma once

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>

#include "kv_store.h"
#include "protocol.h"
#include "serialize.h"
#include "status.h"

namespace ckv {

class KvServer {
 public:
  KvServer(uint port, std::string db_path = "./db") : store_(db_path) {
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd_ < 0) {
      perror("socket error");
      exit(EXIT_FAILURE);
    }

    // 允许 listenfd_ 重用一个在 TIME_WAIT 状态的本地地址和端口
    int reuse = 1;
    if (setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &reuse,
                   sizeof(reuse)) == -1) {
      perror("setsockopt error");
      exit(EXIT_FAILURE);
    }

    memset(&servaddr_, 0, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr_.sin_port = htons(port);

    if (bind(listenfd_, (struct sockaddr*)&servaddr_, sizeof(servaddr_)) < 0) {
      perror("bind error");
      exit(EXIT_FAILURE);
    }

    if (listen(listenfd_, 1024) < 0) {
      perror("listen error");
      exit(EXIT_FAILURE);
    }

    epollfd_ = epoll_create1(0);
    if (epollfd_ < 0) {
      perror("epoll_create1 error");
      exit(EXIT_FAILURE);
    }

    event_.data.fd = listenfd_;
    event_.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, listenfd_, &event_) < 0) {
      perror("epoll_ctl error");
      exit(EXIT_FAILURE);
    }
  }

  ~KvServer() { close(listenfd_); }

  void Serve() {
    while (true) {
      int nready = epoll_wait(epollfd_, events_, MAX_EVENTS, -1);
      if (nready < 0) {
        perror("epoll_wait error");
        continue;
      }
      // TODO: 按理来讲，client 建立连接后，connfd 为 7， client 暂未发送数据，
      // nready 应该是 0 才对，但是实际上 7 即 connfd
      // 上会有可读的事件，进入到 HandleClientRequest(fd);
      // 为什么？？客户端立马关闭也是读事件。可能是这个问题?

      for (int i = 0; i < nready; ++i) {
        int fd = events_[i].data.fd;
        std::cout << "ready: " << nready << ", ready_fd: " << fd << "\n";
        if (fd == listenfd_ && (events_[i].events & EPOLLIN)) {
          HandleNewConnection();
        } else if (fd != listenfd_ && (events_[i].events & EPOLLIN)) {
          HandleClientRequest(fd);
        }
      }
    }
  }

 private:
  static const int MAX_EVENTS = 1024;

  int listenfd_;
  int epollfd_;
  struct sockaddr_in servaddr_;
  struct epoll_event event_;
  struct epoll_event events_[MAX_EVENTS];
  // std::unordered_map<int, std::string> client_fds_;
  KvStore store_;
  std::string path_;

  void HandleNewConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int connfd = accept(listenfd_, (struct sockaddr*)&client_addr, &client_len);
    if (connfd < 0) {
      perror("accept error");
      return;
    }

    // 注册对应读事件 event
    event_.data.fd = connfd;
    event_.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, connfd, &event_) < 0) {
      perror("epoll_ctl error");
      close(connfd);
      return;
    }

    std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr)
              << ", conn_fd: " << connfd << std::endl;
  }

  void HandleClientRequest(int fd) {
    // TODO: 这里假定 kv-client 发送的数据不超过 1024 字节，一次性读完
    // 使用的是 ET 模式，也最好一次性读完数据. => 修改代码
    char read_buf[1024];
    int nread = read(fd, read_buf, sizeof(read_buf));
    if (nread < 0) {
      if (errno == ECONNRESET) {
        std::cerr << "Connection reset by client" << std::endl;
        close(fd);
        epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr);
        // client_fds_.erase(fd);
      } else {
        perror("read error");
      }
      return;
    } else if (nread == 0) {
      std::cout << "Client disconnected, fd: " << fd << std::endl;
      close(fd);
      epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, nullptr);
      // client_fds_.erase(fd);
      return;
    }

    // request_str 保存客户端发送过来的字节流
    std::string request_str;
    request_str.append(read_buf, nread);
    std::string response_str;

    // 反序列化出 Command. -> 解析命令，调用 KvStore 相关操作.
    Command cmd;
    if (deserialize(request_str, cmd) != 0) {
      std::cerr << "Failed to deserialize request: " << request_str
                << std::endl;
      // Return an error response
      Response resp{ResponseType::ERROR, "Invalid request"};
      serialize(resp, response_str);
    } else {
      switch (cmd.type) {
        case CommandType::SET: {
          if (store_.Set(cmd.key, cmd.value).isOk()) {
            Response resp{ResponseType::STATUS, "OK"};
            serialize(resp, response_str);
          } else {
            Response resp{ResponseType::ERROR, "Failed to set key-value pair"};
            serialize(resp, response_str);
          }
          break;
        }
        case CommandType::GET: {
          std::string value;
          if (store_.Get(cmd.key, value).isOk()) {
            Response resp{ResponseType::DATA, value};
            serialize(resp, response_str);
          } else {
            Response resp{ResponseType::ERROR, "Key not found"};
            serialize(resp, response_str);
          }
          break;
        }
        case CommandType::RM: {
          if (store_.Remove(cmd.key).isOk()) {
            Response resp{ResponseType::STATUS, "OK"};
            serialize(resp, response_str);
          } else {
            Response resp{ResponseType::ERROR, "Failed to remove key"};
            serialize(resp, response_str);
          }
          break;
        }
        default: {
          // Return an error response
          Response resp{ResponseType::ERROR, "Invalid command type"};
          serialize(resp, response_str);
          break;
        }
      }
    }
    // Send the response
    ssize_t sent_bytes = send(fd, response_str.c_str(), response_str.size(), 0);
    if (sent_bytes == -1) {
      std::cerr << "Failed to send response: " << strerror(errno) << std::endl;
      // handle the error, e.g. close the connection
      close(fd);
    } else if (sent_bytes < response_str.size()) {
      std::cerr << "Failed to send the entire response: only " << sent_bytes
                << " bytes sent out of " << response_str.size() << " bytes"
                << std::endl;
      // TODO: handle the error, e.g. try to send the remaining data again
    }
  }
};
}  // namespace ckv