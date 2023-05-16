#pragma once
#include <optional>

#include "cfs.pb.h"  // use RCode, ...
#include "chunk_server_manager.h"
#include "file_tree.h"
#include "metadata.pb.h"  // use FileMeta

using cfs::rpc::ChunkInfo;
using cfs::rpc::ChunkServerLocation;
using cfs::rpc::ChunkServerManagerServiceImpl;
using cfs::rpc::RCode;

namespace cfs {
const int CHUNKSIZE = 1024 * 1024;  // 1M TODO: make it common
class MetaDataManager {
 private:
  /* file system tree */
  FileTree* fs_;

  ChunkServerManagerServiceImpl* chunk_server_manager_;  // 单例

 private:
  // TODO: As the master_server, we could use multi-threads to respond to
  // clients. Considering using a concurrent map since multiple clients may
  // attempt to store their files at the same remote file path.
  std::map<std::string, std::vector<uint64_t>> file_to_handles_map_;

  std::map<int64_t, ChunkInfo> handle_to_chunk_info_map_;

  // an atomic uint64 used to assign the unique id for every chunk.
  std::atomic<int64_t> global_chunk_id_{0};

 public:
  MetaDataManager(/* args */);
  ~MetaDataManager();

  // TODO: 实现各种 client 请求的 rpc 接口的功能（来自 MasterServiceImpl）
  // get, put, delete, rename, stat, list, mkdir.
  // 接口设计的纠结之处 => 方法语义上如何与对象的语义匹配
  // TODO: 这一层对应的语义是 GetFileMeta, ModifyFileMeta, AddFileMeta,
  // DeleteFileMeta 更贴切

  std::optional<FileMeta> Stat(std::string_view file_path);

  RCode Rename(std::string_view src_path, std::string_view des_path);

  RCode Mkdir(std::string_view new_dir_path);

  std::optional<std::vector<std::string>> GetAllPaths(
      std::string_view dir_path);

  std::optional<std::vector<FileMeta>> List(std::string_view dir_path);

  std::optional<std::vector<ChunkInfo>> FindLocation(
      const std::string& file_path);

  std::optional<std::vector<ChunkInfo>> Put(const std::string& file_path,
                                            size_t file_size);

 private:
  void SetNewFileHandles(std::string_view file_path, size_t file_size);

  std::vector<ChunkInfo> SetNewHandlesLocations(
      std::vector<uint64_t>& new_handles);

  std::vector<uint64_t> CreateFileChunkHandles(size_t file_size);

  uint64_t GenUniqueHandle();
};
}  // namespace cfs
