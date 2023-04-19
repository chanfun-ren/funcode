#include "chunk_server_manager.h"

namespace cfs {

uint64_t ChunkServerManagerServiceImpl::GenUniqueServerID() {
  return server_id_.fetch_add(1);
}

Status ChunkServerManagerServiceImpl::Register(ServerContext* context,
                                               const RegisterRequest* request,
                                               RegisterReply* response) {
  ChunkServerLocation chunk_server = request->chunk_server_location();
  std::string ip = chunk_server.hostname();
  std::string port = std::to_string(chunk_server.port());
  std::string location = (ip + port);
  if (!chunk_servers_.count(location)) {
    chunk_servers_[location] = GenUniqueServerID();
  }
  // already registered.
  response->set_server_id(chunk_servers_[location]);
  return Status::OK;
}

Status ChunkServerManagerServiceImpl::UnRegister(
    ServerContext* context, const UnRegisterRequest* request,
    UnRegisterReply* response) {
  ChunkServerLocation chunk_server = request->chunk_server_location();
  std::string ip = chunk_server.hostname();
  std::string port = std::to_string(chunk_server.port());
  std::string location = (ip + port);
  uint64_t id = request->server_id();
  if (chunk_servers_.count(location)) {
    if (chunk_servers_[location] != id) {
      // chunk_server 传送过来的 server_id 与 manager 此前分配的不一致
      // 拒绝注销
    } else {
      // 校验成功. => 移除 chunk_server 信息
      chunk_servers_.erase(location);
      chunk_server_info_.erase(id);
    }
  }
  return Status::OK;
}

Status ChunkServerManagerServiceImpl::HeartBeat(ServerContext* context,
                                                const HeartBeatRequest* request,
                                                HeartBeatReply* response) {
  uint64_t id = request->server_id();
  if (chunk_servers_.count(id)) {
    // still alive => update the timestamp; update its statistic
    server_last_alive_time_[id] = time(nullptr);
    chunk_server_info_[id] = request->statistic_info();
  } else {
    // TODO: tell the chunkserver it was offline.
  }
  return Status::OK;
}

void ChunkServerManagerServiceImpl::AllocateChunkForHandle(
    const std::vector<int64_t>& chunk_handles, int replica_num = REPLICANUM) {
  // If the chunk_handles size is N, then we should allocate N * REPLICANUM
  // handles to X chunk_servers, and every of the X chunk_servers has no
  // duplicate handle. So how to pick the X chunk_servers?
  // Strategy0: chunk_server_manager maintain a priority_queue
  // -> pick replica_num chunk_servers with the most space left from the pq
  // -> map the chunk_handles[i] into the pikced chunk_servers
  // -> update the remaining space information of all chunk_servers and loop
  // the above operation for N times.
  // TODO: optimize the strategy.

  // 将 replica_num * chunk_handles.size() 分配到若干个 chunk_server 上
  // 策略0：chunk_server_manager 维护一个容器存储负载信息
  // 1. 从容器中选择剩余空间最多的 replica_num 个 chunk_servers
  // 2. 将 chunk_handles[i] 映射到选定的 chunk_servers 上，更新它们
  // 的剩余空间信息
  // 3. 循环执行上述操作 N 次。

  // TODO:考虑并发性，怎么保证 manager 中的优先队列 pq_
  // 的数据是最新/较新的数据。
  int n = chunk_handles.size();
  for (int i = 0; i < n; ++i) {
    // 0. select REPLICANUM chunkservers from the container.
    // 1. update the servers' statistic info (map) directly.
    // 2. map the chunkhandle to the servers.
    for (int j = 0; j < replica_num; ++j) {
      // ChunkServerStatisticInfo& server_info;
      uint64_t max_available_size = 0;
      auto max_chunkserver = chunk_server_info_.end();
      for (auto it = chunk_server_info_.begin(); it != chunk_server_info_.end();
           ++it) {
        if (it->second.size_available_bytes() > max_available_size) {
          max_chunkserver = it;
          max_available_size = it->second.size_available_bytes();
        }
      }
      ChunkServerStatisticInfo& server_info = max_chunkserver->second;
      server_info.add_chunk_handles(chunk_handles[i]);
      uint64_t current_size = server_info.size_available_bytes();
      server_info.set_available_bytes(current_size - CHUNKSIZE);
      server_info.set_used_bytes(current_size + CHUNKSIZE);
      // TODO: update metadata_manager_->handle_locations_map_;
      // handle_locations_map_[chunk_handles[i]].push_back(server_info.chunk_server_location());
    }
  }
}

ChunkServerManager::ChunkServerManager(/* args */) {}

ChunkServerManager::~ChunkServerManager() {}
}  // namespace cfs
