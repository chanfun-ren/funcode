#include "master_service.h"

// #include <algorithm>

namespace cfs {
namespace rpc {

MasterServiceImpl::MasterServiceImpl() {
  metadata_manager_ = new MetaDataManager();
}

MasterServiceImpl::~MasterServiceImpl() { delete metadata_manager_; }

Status MasterServiceImpl::Stat(ServerContext *ctx, const StatRequest *req,
                               StatReply *reply) {
  // metadata_ma_->Stat(); -> file_tree.Getnode();
  std::string file_path = req->file_path();
  std::optional<FileMeta> file_meta = metadata_manager_->Stat(file_path);

  if (file_meta.has_value()) {
    reply->set_ret_code(RCode::OK);
    FileMeta *fm_p = reply->mutable_file_meta();
    fm_p->set_file_path(file_meta.value().file_path());
    fm_p->set_file_size(file_meta.value().file_size());
  } else {
    reply->set_ret_code(RCode::FILE_NOTFOUND);
  }
  return Status::OK;
}

Status MasterServiceImpl::Rename(ServerContext *ctx, const RenameRequest *req,
                                 RenameReply *reply) {
  std::string src_path = req->src_path();
  std::string des_path = req->des_path();
  RCode res_code = metadata_manager_->Rename(src_path, des_path);

  reply->set_ret_code(res_code);
  return Status::OK;
}
Status MasterServiceImpl::Mkdir(ServerContext *ctx, const MkdirRequest *req,
                                MkdirReply *reply) {
  std::string dir_path = req->dir_path();
  RCode res_code = metadata_manager_->Mkdir(dir_path);

  reply->set_ret_code(res_code);
  return Status::OK;
}

Status MasterServiceImpl::List(ServerContext *ctx, const ListRequest *req,
                               ListReply *reply) {
  std::string dir_path = req->dir_path();

  std::optional<std::vector<FileMeta>> files_meta =
      metadata_manager_->List(dir_path);
  if (files_meta.has_value()) {
    reply->set_ret_code(RCode::OK);
    for (FileMeta &meta : files_meta.value()) {
      // TODO: Ugly. Make the code more elegant and efficient.
      FileMeta *tmp = reply->add_files_meta();
      tmp->set_file_path(meta.file_path());
      tmp->set_file_size(meta.file_size());
    }
  } else {
    reply->set_ret_code(RCode::FILE_NOTFOUND);
  }
  return Status::OK;
}

Status MasterServiceImpl::GetAllPaths(ServerContext *ctx,
                                      const GetAllPathsRequest *req,
                                      GetAllPathsReply *reply) {
  std::string dir_path = req->dir_path();
  std::optional<std::vector<std::string>> file_paths =
      metadata_manager_->GetAllPaths(dir_path);
  if (file_paths.has_value()) {
    // set reply
    std::vector<std::string> paths = file_paths.value();
    for (std::string &path : paths) {
      // std::cout << __func__ << "add_file_paths-" << path << "\n";
      reply->add_file_paths(path);
    }
    reply->set_ret_code(RCode::OK);
  } else {
    reply->set_ret_code(RCode::FILE_NOTFOUND);
  }
  return Status::OK;
}

Status MasterServiceImpl::FindLocation(ServerContext *ctx,
                                       const FindLocationRequest *req,
                                       FindLocationReply *reply) {
  std::string file_path = req->file_path();
  std::optional<std::vector<ChunkInfo>> info =
      metadata_manager_->FindLocation(file_path);
  if (info.has_value()) {
    std::vector<ChunkInfo> data = info.value();
    *reply->mutable_chunks_info() = {data.begin(), data.end()};
    reply->set_ret_code(RCode::OK);
  } else {
    repy->set_ret_code(RCode::FILE_NOTFOUND);
  }
  return Status::OK;
}

Status MasterServiceImpl::GetWriteLocation(ServerContext *ctx,
                                           const GetWriteLocationRequest *req,
                                           GetWriteLocationReply *reply) {
  std::string file_path = req->file_path();
  size_t file_size = req->write_data_size();

  std::optional<std::vector<ChunkInfo>> info =
      metadata_manager_->GetWriteLocation(file_path, file_size);
  if (info.has_value()) {
    std::vector<ChunkInfo> data = info.value();
    *reply->mutable_chunks_info() = {data.begin(), data.end()};
    reply->set_ret_code(RCode::OK);
  } else {
    repy->set_ret_code(RCode::FILE_EXIST);
  }
  return Status::OK;
}

}  // namespace rpc
}  // namespace cfs
