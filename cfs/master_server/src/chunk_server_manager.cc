#include "chunk_server_manager.h"

namespace cfs {

namespace rpc {
uint64_t ChunkServerManagerServiceImpl::GenUniqueServerID() {
  return server_id_.fetch_add(1);
}

Status ChunkServerManagerServiceImpl::Register(ServerContext* context,
                                               const RegisterRequest* request,
                                               RegisterReply* response) {
  ChunkServerLocation chunk_server = request->chunk_server_location();

  for (const auto& server : servers_) {
    if (server.location.hostname() == chunk_server.hostname() &&
        server.location.port() == chunk_server.port()) {
      // already registered, return the existing server ID
      response->set_server_id(server.id);
      return Status::OK;
    }
  }

  // generate a new server ID and register the chunk server
  uint64_t id = GenUniqueServerID();
  response->set_server_id(id);
  servers_.push_back({id, std::time(nullptr), ChunkServerStatisticInfo(),
                      chunk_server});  // TODO: `ChunkServerStatisticInfo()`
  return Status::OK;
}

Status ChunkServerManagerServiceImpl::UnRegister(
    ServerContext* context, const UnRegisterRequest* request,
    UnRegisterReply* response) {
  ChunkServerLocation chunk_server_addr = request->chunk_server_location();
  uint64_t id = request->server_id();

  auto it = std::find_if(
      servers_.begin(), servers_.end(),
      [&chunk_server_addr](const ChunkServerItem& server) {
        return server.location.hostname() == chunk_server_addr.hostname() &&
               server.location.port() == chunk_server_addr.port();
      });
  if (it == servers_.end()) {
    // not registered
    return Status::OK;
  }
  if (it->id != id) {
    // server ID does not match, reject the unregister request
    // 校验失败，拒绝注销
    // TODO:
  } else {
    // unregister the chunk server
    servers_.erase(it);
  }
  return Status::OK;
}

Status ChunkServerManagerServiceImpl::HeartBeat(ServerContext* context,
                                                const HeartBeatRequest* request,
                                                HeartBeatReply* response) {
  uint64_t id = request->server_id();
  std::cout << "server_id: " << id << "\n";
  ChunkServerStatisticInfo chunk_server_info = request->statistic_info();
  // 输出 chunk_server_info 对象的内容
  /*
  std::cout << "chunk_handles:";
  for (int i = 0; i < chunk_server_info.chunk_handles_size(); i++) {
    std::cout << " " << chunk_server_info.chunk_handles(i);
  }
  std::cout << std::endl;
  std::cout << "used_bytes: " << chunk_server_info.used_bytes() << std::endl;
  std::cout << "available_bytes: " << chunk_server_info.available_bytes()
            << std::endl;
  std::cout << "chunk_server_location: "
            << chunk_server_info.chunk_server_location().hostname() << ":"
            << chunk_server_info.chunk_server_location().port() << std::endl;
  */

  auto it = std::find_if(
      servers_.begin(), servers_.end(),
      [&id](const ChunkServerItem& server) { return server.id == id; });
  if (it != servers_.end()) {
    // still alive => update the timestamp; update its statistic
    it->server_last_alive_time = time(nullptr);
    it->chunk_server_info = request->statistic_info();
  } else {
    // TODO: tell the chunkserver it was offline.
  }
  return Status::OK;
}

std::vector<ChunkServerLocation>
ChunkServerManagerServiceImpl::GetTopKChunkServerLocations(int k) {
  std::vector<ChunkServerItem> sorted_servers = servers_;
  std::sort(sorted_servers.begin(), sorted_servers.end(),
            [](const ChunkServerItem& a, const ChunkServerItem& b) {
              return a.chunk_server_info.available_bytes() >
                     b.chunk_server_info.available_bytes();
            });

  std::vector<ChunkServerLocation> top_k_locations;
  for (int i = 0; i < k && i < sorted_servers.size(); i++) {
    top_k_locations.push_back(sorted_servers[i].location);
  }
  return top_k_locations;
}

/*
void ChunkServerManagerServiceImpl::AllocateServerForHandle(
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
        if (it->second.available_bytes() > max_available_size) {
          max_chunkserver = it;
          max_available_size = it->second.available_bytes();
        }
      }
      ChunkServerStatisticInfo& server_info = max_chunkserver->second;
      server_info.add_chunk_handles(chunk_handles[i]);
      uint64_t current_size = server_info.available_bytes();
      server_info.set_available_bytes(current_size - CHUNKSIZE);
      server_info.set_used_bytes(current_size + CHUNKSIZE);
      // TODO: update metadata_manager_->handle_locations_map_;
      //
      handle_locations_map_[chunk_handles[i]].push_back(
          server_info.chunk_server_location());
    }
  }
}
*/

}  // namespace rpc
}  // namespace cfs
