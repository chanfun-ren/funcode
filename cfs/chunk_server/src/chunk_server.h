#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "cfs.grpc.pb.h"

using cfs::rpc::ChunkServerLocation;
using cfs::rpc::ChunkServerManagerService;
using cfs::rpc::ChunkServerStatisticInfo;
using cfs::rpc::HeartBeatReply;
using cfs::rpc::HeartBeatRequest;
using cfs::rpc::RegisterReply;
using cfs::rpc::RegisterRequest;
using cfs::rpc::UnRegisterReply;
using cfs::rpc::UnRegisterRequest;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

namespace cfs {
const int HEARTBEAT_DURATION_SECONDS = 3;

class ChunkServer {
 private:
  std::unique_ptr<ChunkServerManagerService::Stub> stub_;
  std::vector<uint64_t> chunk_handles_;
  std::string hostname_;
  uint32_t port_;
  uint64_t used_bytes_ = 0;
  uint64_t available_bytes_ = 1024 * 1024 * 1024;
  uint64_t self_id_;

  std::thread heart_beat_thread_;

 public:
  ChunkServer(std::shared_ptr<Channel> channel, const std::string& hostname,
              uint32_t port)
      : stub_(ChunkServerManagerService::NewStub(channel)),
        hostname_(hostname),
        port_(port) {}

  void StartHeartBeat();

  Status HeartBeat();

  Status Register();

  Status UnRegister();
};
}  // namespace cfs
