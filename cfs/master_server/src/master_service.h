#pragma once

// #include <grpcpp/grpcpp.h>

#include "cfs.grpc.pb.h"
#include "cfs.pb.h"
#include "metadata_manager.h"

using grpc::ServerContext;
using grpc::Status;

namespace cfs {
namespace rpc {
class MasterServiceImpl final : public MasterService::Service {
 private:
  MetaDataManager *metadata_manager_;  // TODO: 单例

 public:
  MasterServiceImpl();
  ~MasterServiceImpl();

  Status Stat(ServerContext *ctx, const StatRequest *req,
              StatReply *reply) override;

  Status Rename(ServerContext *ctx, const RenameRequest *req,
                RenameReply *reply) override;
  Status Mkdir(ServerContext *ctx, const MkdirRequest *req,
               MkdirReply *reply) override;
  Status List(ServerContext *ctx, const ListRequest *req,
              ListReply *reply) override;
  Status GetAllPaths(ServerContext *ctx, const GetAllPathsRequest *req,
                     GetAllPathsReply *reply) override;

  Status FindLocation(ServerContext *ctx, const FindLocationRequest *req,
                      FindLocationReply *reply) override;

  Status GetWriteLocation(ServerContext *ctx,
                          const GetWriteLocationRequest *req,
                          GetWriteLocationReply *reply) override;
};
}  // namespace rpc

}  // namespace cfs
