#include <iostream>

#include "chunk_server.h"
#include "common/logging.h"
#include "common/utils.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

namespace cfs {

// ChunkServerServiceImpl

Status ChunkServerServiceImpl::ReadChunk(ServerContext* context,
                                         const ReadChunkRequest* request,
                                         ReadChunkReply* reply) {
  const uint64_t chunk_handle = request->chunk_handle();
  LOG_DEBUG << "ReadChunk " << chunk_handle;

  std::lock_guard<std::mutex> lock(mutex_);

  auto it = chunks_.find(chunk_handle);
  if (it == chunks_.end()) {
    LOG_ERROR << "Chunk not found: " << chunk_handle;
    return Status(grpc::StatusCode::NOT_FOUND, "Chunk not found");
  }

  const Chunk& chunk = it->second;
  const uint64_t offset = request->offset();
  const uint64_t size = request->size();
  if (offset + size > chunk.size()) {
    LOG_ERROR << "Invalid read request: offset=" << offset << ", size=" << size
              << ", chunk size=" << chunk.size();
    return Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid read request");
  }

  reply->set_data(chunk.data().substr(offset, size));

  return Status::OK;
}

Status ChunkServerServiceImpl::WriteChunk(ServerContext* context,
                                          const WriteChunkRequest* request,
                                          WriteChunkReply* reply) {
  const uint64_t chunk_handle = request->chunk_handle();
  LOG_DEBUG << "WriteChunk " << chunk_handle;

  std::lock_guard<std::mutex> lock(mutex_);

  auto it = chunks_.find(chunk_handle);
  if (it == chunks_.end()) {
    LOG_ERROR << "Chunk not found: " << chunk_handle;
    return Status(grpc::StatusCode::NOT_FOUND, "Chunk not found");
  }

  const uint64_t offset = request->offset();
  const uint64_t size = request->data().size();
  if (offset + size > it->second.size()) {
    LOG_ERROR << "Invalid write request: offset=" << offset << ", size=" << size
              << ", chunk size=" << it->second.size();
    return Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid write request");
  }

  Chunk& chunk = it->second;
  chunk.set_data(chunk.data().replace(offset, size, request->data()));

  return Status::OK;
}

}  // namespace cfs