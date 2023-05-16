#pragma once

#include <grpcpp/grpcpp.h>

#include <fstream>
#include <memory>

#include "cfs.grpc.pb.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"  // or "../stdout_sinks.h" if no colors needed
#include "spdlog/spdlog.h"

// To check the return status of a RPC invovation.
#define CHECK_STATUS(rpc_name, status)                                   \
  if (!status.ok()) {                                                    \
    spdlog::error("{} failed. err code: {}. err message: {}.", rpc_name, \
                  status.error_code(), status.error_message());          \
    return;                                                              \
  }

// #define CHECK_STATUS(rpc_name, status)                                        \
//   if (!status.ok()) {                                                         \
//     std::cerr << "Error: " << __FILE__ << ":" << __LINE__ << ": " << __func__ \
//               << "() - rpc name: " << rpc_name                                \
//               << ". Error code: " << status.error_code()                      \
//               << ", message: " << status.error_message() << std::endl;        \
//     std::cout << "Fail to " << __func__ << std::endl;                         \
//     return;                                                                   \
//   }
// TODO: replace by logger

using cfs::rpc::MasterService;

namespace cfs {
class Client {
 private:
  std::unique_ptr<MasterService::Stub> cli_master_stub_;

 public:
  Client(std::shared_ptr<grpc::Channel> channel);
  void Stat(const std::string& file_path) const;
  void Rename(const std::string& src_path, const std::string& des_path) const;
  void Mkdir(const std::string& dir_path) const;
  void List(const std::string& dir_path) const;

  void Get(const std::string& remote_file_path,
           const std::string& local_file_path) const;
  void Put(const std::string& remote_file_path,
           const std::string& local_file_path) const;

 private:
  size_t GetFileSize(const std::string& file_path) const;
};
}  // namespace cfs
