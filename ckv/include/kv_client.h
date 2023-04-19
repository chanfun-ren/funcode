#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "protocol.h"
#include "serialize.h"
#include "status.h"

namespace ckv {

class KvClient {
 public:
  KvClient(const char* addr, int port) {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ < 0) {
      perror("socket error");
      exit(EXIT_FAILURE);
    }

    memset(&servaddr_, 0, sizeof(servaddr_));
    servaddr_.sin_family = AF_INET;
    servaddr_.sin_port = htons(port);
    if (inet_pton(AF_INET, addr, &servaddr_.sin_addr) <= 0) {
      perror("inet_pton error");
      exit(EXIT_FAILURE);
    }

    int ret = connect(sockfd_, (struct sockaddr*)&servaddr_, sizeof(servaddr_));
    if (ret < 0) {
      perror("connect error");
      // exit(EXIT_FAILURE);
    } else if (ret == 0) {
      std::cout << "connected to sever. fd: " << sockfd_ << "\n";
    }
  }

  KvClient(const std::string& addr)
      : KvClient(addr.substr(0, addr.find(':')).c_str(),
                 std::stoi(addr.substr(addr.find(':') + 1))) {}

  ~KvClient() { close(sockfd_); }

  Status ExecCmd(Command& cmd, Response& response) {
    // 序列化请求
    std::string request_str;
    if (serialize(cmd, request_str) != 0) {
      std::cerr << "serialize error" << std::endl;
      return Status::Corruption("fail to serialize to network");
    }

    // 发送数据
    std::string response_str;
    Status send_ret = SendRequest(request_str, response_str);
    if (!send_ret.isOk()) {
      return send_ret;
    }

    // 解析回复
    if (deserialize(response_str, response) != 0) {
      std::cerr << "deserialize error" << std::endl;
      return Status::Corruption("fail to deserialize from network");
    }

    return Status::Ok();
  }

 private:
  uint sockfd_;
  struct sockaddr_in servaddr_;

  Status SendRequest(const std::string& request_str,
                     std::string& response_str) {
    char response_buf[1024];
    ssize_t n;
    int w_ret = write(sockfd_, request_str.c_str(), request_str.size());
    if (w_ret == 0) {
      return Status::NetworkError("server closed");
    }
    if (w_ret == -1) {
      // perror("write error");
      return Status::NetworkError(std::string("write error: ") +
                                  strerror(errno));
    }
    if (w_ret != (ssize_t)request_str.size()) {
      // perror("write error");
      return Status::NetworkError("write error: only wrote partial data.");
    }

    n = read(sockfd_, response_buf, sizeof(response_buf) - 1);
    if (n < 0) {
      // perror("read error");
      return Status::NetworkError("read error");
    }
    response_buf[n] = '\0';
    response_str = response_buf;

    return Status::Ok();
  }
};

}  // namespace ckv