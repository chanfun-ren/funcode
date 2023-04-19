#include <iostream>

#include "cfs.grpc.pb.h"
#include "cfs.pb.h"
#include "cilent.h"

using cfs::FileMeta;
using cfs::rpc::ChunkInfo;
using cfs::rpc::ChunkServerLocation;
using cfs::rpc::FindLocationReply;
using cfs::rpc::FindLocationRequest;
using cfs::rpc::GetAllPathsReply;
using cfs::rpc::GetAllPathsRequest;
using cfs::rpc::GetWriteLocationReply;
using cfs::rpc::GetWriteLocationRequest;
using cfs::rpc::ListReply;
using cfs::rpc::ListRequest;
using cfs::rpc::MasterService;
using cfs::rpc::MkdirReply;
using cfs::rpc::MkdirRequest;
using cfs::rpc::RCode;
using cfs::rpc::RenameReply;
using cfs::rpc::RenameRequest;
using cfs::rpc::StatReply;
using cfs::rpc::StatRequest;

using grpc::ClientContext;
using grpc::Status;

// SPDLOG_LOGGER_INFO(console, "log with source info");
// SPDLOG_INFO("global log with source info");
// console->info("source info is not printed");
// Console: "[source ] [function ] [line] source info is not printed"

namespace cfs {

Client::Client(std::shared_ptr<grpc::Channel> channel) {
  cli_master_stub_ = MasterService::NewStub(channel);
}

void Client::Stat(const std::string& file_path) const {
  // if (file_path.empty()) {  // for debugging.
  //   std::cout << "path can not be empty!!!\n";
  //   return;
  // }
  StatRequest req;
  StatReply reply;
  ClientContext ctx;
  req.set_file_path(file_path);
  Status status = cli_master_stub_->Stat(&ctx, req, &reply);

  CHECK_STATUS("Stat", status);

  // rpc status is ok? => check ret code.
  if (reply.ret_code() == RCode::OK) {
    FileMeta file_meta = reply.file_meta();
    std::string file_path = file_meta.file_path();
    uint64_t file_size = file_meta.file_size();
    std::cout << "file path: " << file_path << ", size: " << file_size << "\n";
  } else if (reply.ret_code() == RCode::FILE_NOTFOUND) {
    std::cout << file_path << " No such file or directory...\n";
  }  // TODO: other ret code.
  spdlog::trace("{} {} done", __func__, file_path);
}

void Client::Rename(const std::string& src_path,
                    const std::string& des_path) const {
  ClientContext ctx;
  RenameRequest req;
  RenameReply reply;
  req.set_src_path(src_path);
  req.set_des_path(des_path);

  Status status = cli_master_stub_->Rename(&ctx, req, &reply);
  CHECK_STATUS("Rename", status);

  if (reply.ret_code() == RCode::OK) {
    // do nothing: "no news is good news".
  } else if (reply.ret_code() == RCode::FILE_NOTFOUND) {
    std::cout << src_path << " No such file or directory.\n";
  } else if (reply.ret_code() == RCode::FILE_EXIST) {
    std::cout << des_path << " Path already exists.\n";
  }
  spdlog::trace("{} {} to {} done", __func__, src_path, des_path);
}

void Client::Mkdir(const std::string& dir_path) const {
  ClientContext ctx;
  MkdirRequest req;
  MkdirReply reply;
  req.set_dir_path(dir_path);

  Status status = cli_master_stub_->Mkdir(&ctx, req, &reply);
  CHECK_STATUS("Mkdir", status);

  if (reply.ret_code() == RCode::OK) {
    // do nothing: "no news is good news".
  } else if (reply.ret_code() == RCode::FILE_EXIST) {
    std::cout << dir_path << " Path already exists.\n";
  }
  spdlog::trace("{} {} done", __func__, dir_path);
}

void Client::List(const std::string& dir_path) const {
  ClientContext ctx;
  ListRequest req;
  ListReply reply;
  req.set_dir_path(dir_path);

  Status status = cli_master_stub_->List(&ctx, req, &reply);
  CHECK_STATUS("GetAllPaths", status);
  if (reply.ret_code() == RCode::OK) {
    for (auto& fmeta : reply.files_meta()) {
      std::cout << fmeta.file_path() << ", size: " << fmeta.file_size() << "\n";
    }
  } else if (reply.ret_code() == RCode::FILE_NOTFOUND) {
    std::cout << dir_path << " No such file or directory.\n";
  }
  spdlog::trace("{} {} done", __func__, dir_path);
}

void Client::Get(const std::string& file_path) const {
  FindLocationRequest req;
  FindLocationReply reply;
  ClientContext ctx;
  req.set_file_path(file_path);
  Status status = cli_master_stub_->FindLocation(&ctx, req, &reply);
  CHECK_STATUS("FindLocation", status);

  if (reply.ret_code() == RCode::OK) {
    // 解析出各个 Chunk 的 handles, chunk_server_locations 信息(ChunkInfo)
    // 向 chunk_server 发起请求
    for (const ChunkInfo& chunk_info : reply.chunks_info()) {
      uint64_t handle = chunk_info.chunk_handle();
      for (ChunkServerLocation& server : chunk_info.replica_addrs()) {
        std::cout << server.hostname() << ": " << server.port() << "\t";
        // TODO: Read Chunk
      }
    }
  } else if (reply.ret_code() == RCode::FILE_NOTFOUND) {
    std::cout << file_path << " No such file.\n";
  }
}

void Client::Put(const std::string& file_path) const {
  size_t file_size = GetFileSize(file_path);
  GetWriteLocationRequest req;
  GetWriteLocationReply reply;
  ClientContext ctx;
  req.set_file_path(file_path);
  req.set_write_data_size(file_size);
  Status status = cli_master_stub_->GetWriteLocation(&ctx, req, &reply);
  CHECK_STATUS("GetWriteLocation", status);

  if (reply.ret_code() == RCode::OK) {
    // 解析出各个 Chunk 的 handles, chunk_server_locations 信息(ChunkInfo)
    // 向 chunk_server 发起请求
    for (ChunkInfo& chunk_info : reply.chunks_info()) {
      uint64_t handle = chunk_info.chunk_handle();
      for (ChunkServerLocation& server : chunk_info.replica_addrs()) {
        std::cout << server.hostname() << ": " << server.port() << "\t";
        // TODO: Write Chunk.
      }
    }
  } else if (reply.ret_code() == RCode::FILE_EXIST) {
    std::cout << file_path << " already exists.\n";
  }
}

size_t Client::GetFileSize(const std::string& filename) const {
  std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  // actual type: std::ifstream::pos_type
  size_t n = in.tellg();
  in.close();
  return n;
}
}  // namespace cfs
