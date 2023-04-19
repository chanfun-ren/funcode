#pragma once
#include <ctime>
#include <map>
#include <queue>
#include <set>

#include "cfs.grpc.pb.h"
#include "cfs.pb.h"

const int REPLICANUM = 3;

using cfs::rpc::ChunkServerStatisticInfo;
using grpc::ServerContext;
using grpc::Status;

namespace cfs {

class ChunkServerManagerServiceImpl final
    : public ChunkServerManagerServiceImpl {
 public:
  Status Register(ServerContext* context, const RegisterRequest* request,
                  RegisterReply* response) override;

  Status UnRegister(ServerContext* context, const UnRegisterRequest* request,
                    UnRegisterReply* response) override;

  Status HeartBeat(ServerContext* context, const HeartBeatRequest* request,
                   HeartBeatReply* response) override;

 public:
  void AllocateChunkForHandle(const std::vector<int64_t>& chunk_handles,
                              int replica_num);

 private:
  uint64_t GenUniqueServerID();

 private:
  // location => server_id
  std::map<std::string, uint64_t> chunk_servers_;

  // stores the alive chunkservers' statistic info
  std::map<uint64_t, ChunkServerStatisticInfo> chunk_server_info_;

  // stores the chunkserver's last time_stamp
  std::map<uint64_t, std::time_t> server_last_alive_time_;

  // chunkserver id
  std::atomic<int64_t> server_id_{0};

  // sorted container stores the chunkservers' load info.
  // using IterType = std::map<uint64_t, ChunkServerStatisticInfo>::iterator;
  // 剩余空间大的 chunkserver 优先
  // std::set<uint64_t> chunkserver_load_priority_;
  // return chunk_server_info_[id1].available_bytes() <
  // chunk_server_info_[id2].available_bytes()
};

class ChunkServerManager {
 public:
  ChunkServerManager(/* args */);
  ~ChunkServerManager();

 private:
};
}  // namespace cfs
