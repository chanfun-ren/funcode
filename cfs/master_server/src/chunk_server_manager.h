#pragma once
#include <ctime>
#include <map>
#include <queue>
#include <set>

#include "cfs.grpc.pb.h"
#include "cfs.pb.h"

const int REPLICANUM = 3;

using cfs::rpc::ChunkServerLocation;
using cfs::rpc::ChunkServerStatisticInfo;
using cfs::rpc::HeartBeatReply;
using cfs::rpc::HeartBeatRequest;
using cfs::rpc::RegisterReply;
using cfs::rpc::RegisterRequest;
using cfs::rpc::UnRegisterReply;
using cfs::rpc::UnRegisterRequest;

using grpc::ServerContext;
using grpc::Status;

namespace cfs {
namespace rpc {

const int CHUNKSIZE = 1024 * 1024;  // TODO: make it common

class ChunkServerManagerServiceImpl final
    : public ChunkServerManagerService::Service {
 public:
  Status Register(ServerContext* context, const RegisterRequest* request,
                  RegisterReply* response) override;

  Status UnRegister(ServerContext* context, const UnRegisterRequest* request,
                    UnRegisterReply* response) override;

  Status HeartBeat(ServerContext* context, const HeartBeatRequest* request,
                   HeartBeatReply* response) override;

  // 返回剩余空间最多的 k 个 chunkserver 的 location 信息
  std::vector<ChunkServerLocation> GetTopKChunkServerLocations(int k);

 private:
  uint64_t GenUniqueServerID();

 private:
  struct ChunkServerItem {
    uint64_t id;
    std::time_t server_last_alive_time;
    ChunkServerStatisticInfo chunk_server_info;
    ChunkServerLocation location;
  };

  std::vector<ChunkServerItem> servers_;

  // generate chunkserver unique id
  std::atomic<int64_t> server_id_{0};
};

}  // namespace rpc
}  // namespace cfs
