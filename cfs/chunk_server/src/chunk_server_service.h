#pragma once

#include <grpcpp/grpcpp.h>

#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "cfs.grpc.pb.h"
#include "cfs.pb.h"

using cfs::rpc::Chunk;
using cfs::rpc::ChunkInfo;
using cfs::rpc::ChunkServerLocation;
using cfs::rpc::ChunkServerManagerService;
using cfs::rpc::ChunkServerService;
using cfs::rpc::ChunkServerStatisticInfo;
using cfs::rpc::HeartBeatReply;
using cfs::rpc::HeartBeatRequest;
using cfs::rpc::ReadChunkReply;
using cfs::rpc::ReadChunkRequest;
using cfs::rpc::WriteChunkReply;
using cfs::rpc::WriteChunkRequest;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

namespace cfs {

class ChunkServerServiceImpl final : public ChunkServerService {
 public:
  ChunkServerServiceImpl(const std::string& hostname, uint32_t port)
      : hostname_(hostname), port_(port), {}

  Status ReadChunk(ServerContext* context, const ReadChunkRequest* request,
                   ReadChunkReply* reply) override;

  Status WriteChunk(ServerContext* context, const WriteChunkRequest* request,
                    WriteChunkReply* reply) override;

 private:
  std::string hostname_;
  uint32_t port_;
  uint64_t server_id_ = 0;

  std::map<uint64_t, Chunk> chunks_;

  std::atomic<bool> stop_{false};

  std::mutex mutex_;
};

}  // namespace cfs