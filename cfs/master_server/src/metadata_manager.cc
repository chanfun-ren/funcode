#include "metadata_manager.h"
namespace cfs {

MetaDataManager::MetaDataManager() { fs_ = new FileTree(); }
MetaDataManager::~MetaDataManager() { delete fs_; }

std::optional<FileMeta> MetaDataManager::Stat(std::string_view file_path) {
  FileNode* file_node = fs_->FindNode(file_path);
  std::cout << "find:" << file_path << "... ";
  if (file_node == nullptr) {
    std::cout << "do not find" << file_path << "\n";
    return std::nullopt;
  }

  FileMeta file_meta;
  file_meta.set_file_path(file_node->file_path);
  file_meta.set_file_size(file_node->file_size);
  return file_meta;
}

RCode MetaDataManager::Rename(std::string_view src_path,
                              std::string_view des_path) {
  FileNode* src_node = fs_->FindNode(src_path);
  FileNode* des_node = fs_->FindNode(des_path);
  if (src_node == nullptr) {
    return RCode::FILE_NOTFOUND;
  }
  if (des_node != nullptr) {
    return RCode::FILE_EXIST;
  }

  // Get file metadata information, currently only retrieving file size
  size_t size = src_node->file_size;
  fs_->DeleteNode(src_node->file_path);
  fs_->CreateNode(des_path, size);
  return RCode::OK;
}

RCode MetaDataManager::Mkdir(std::string_view new_dir_path) {
  FileNode* dir_node = fs_->FindNode(new_dir_path);
  if (dir_node != nullptr) {
    return RCode::FILE_EXIST;
  }

  // TODO: CreateNode 前检查新路径是 "/a/b/c" 的合法形式 -> `util.h`
  const size_t DIR_FILE_PLACEHODLER_SIZE = 1;
  // fs_->CreateNode(new_dir_path, DIR_FILE_PLACEHODLER_SIZE);
  std::string new_dir_file{new_dir_path.begin(), new_dir_path.end()};
  Put(new_dir_file, DIR_FILE_PLACEHODLER_SIZE);
  return RCode::OK;
}

std::optional<std::vector<std::string>> MetaDataManager::GetAllPaths(
    std::string_view dir_path) {
  std::cout << "GetAllPaths begin:...\n";
  FileNode* dir_node = fs_->FindNode(dir_path);
  if (dir_node == nullptr) {
    return std::nullopt;
  }

  std::vector<std::string> children_paths;
  for (FileNode* node : dir_node->children) {
    children_paths.push_back(std::move(node->file_path));
  }
  std::cout << "GetAllPaths end. ";
  fs_->PrintTree();
  return children_paths;
}

std::optional<std::vector<FileMeta>> MetaDataManager::List(
    std::string_view dir_path) {
  FileNode* dir_node = fs_->FindNode(dir_path);
  if (dir_node == nullptr) {
    return std::nullopt;
  }
  std::vector<FileMeta> list_res;
  for (FileNode* node : dir_node->children) {
    FileMeta fmeta;
    fmeta.set_file_path(node->file_path);
    fmeta.set_file_size(node->file_size);
    list_res.push_back(std::move(fmeta));
  }
  return list_res;  // maybe empty.
}

std::optional<std::vector<ChunkInfo>> MetaDataManager::FindLocation(
    const std::string& file_path) {
  auto it = file_to_handles_map_.find(file_path);
  if (it == file_to_handles_map_.end()) {
    return std::nullopt;
  }
  std::vector<uint64_t> handles = it->second;
  std::vector<ChunkInfo> res;
  for (uint64_t handle : handles) {
    ChunkInfo& chunk_info = handle_to_chunk_info_map_[handle];
    res.push_back(chunk_info);
  }
  return res;
}

std::optional<std::vector<ChunkInfo>> MetaDataManager::Put(
    const std::string& file_path, size_t file_size) {
  FileNode* file_node = fs_->FindNode(file_path);
  if (file_node != nullptr) {  // 文件已经存在
    return std::nullopt;
  }

  fs_->CreateNode(file_path, file_size);

  SetNewFileHandles(file_path, file_size);  // update `file_to_handles_map_`
  std::vector<uint64_t> handles = file_to_handles_map_[file_path];
  return SetNewHandlesLocations(handles);
}

void MetaDataManager::SetNewFileHandles(std::string_view file_path,
                                        size_t file_size) {
  fs_->CreateNode(file_path, file_size);
  std::vector<uint64_t> chunk_handles = CreateFileChunkHandles(file_size);
  std::string file(file_path);
  file_to_handles_map_[file] = chunk_handles;
}

std::vector<ChunkInfo> MetaDataManager::SetNewHandlesLocations(
    std::vector<uint64_t>& new_handles) {
  // 根据 chunkserver 负载，chunk_server_manager 为 chunk_server 分配
  // chunk_handles 根据 chunk_server_manager
  // 选出若干个/REPLICANUM个存储空间足够的 chunk_server
  std::vector<ChunkInfo> res;
  for (uint64_t handle : new_handles) {
    std::vector<ChunkServerLocation> chunk_server_locations =
        chunk_server_manager_->GetTopKChunkServerLocations(REPLICANUM);
    // TODO: update chunk_server available bytes
    ChunkInfo chunk_info;
    chunk_info.set_chunk_handle(handle);
    for (const auto& location : chunk_server_locations) {
      ChunkServerLocation* replica_addr = chunk_info.add_replica_addrs();
      replica_addr->CopyFrom(location);
    }
    handle_to_chunk_info_map_[handle] = chunk_info;
    res.push_back(chunk_info);
  }
  return res;
}

std::vector<uint64_t> MetaDataManager::CreateFileChunkHandles(
    size_t file_size) {
  int chunk_handles_num = (file_size + CHUNKSIZE - 1) / CHUNKSIZE;
  std::vector<uint64_t> chunk_handles;
  for (int i = 0; i < chunk_handles_num; ++i) {
    chunk_handles.push_back(GenUniqueHandle());
  }
  return chunk_handles;
}

uint64_t MetaDataManager::GenUniqueHandle() {
  return global_chunk_id_.fetch_add(1);
}

}  // namespace cfs
