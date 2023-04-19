#include "client.h"

#include "metadata.pb.h"

// #include "easylogging++.h"

using cfs::rpc::DeleteReply;
using cfs::rpc::DeleteRequest;
using cfs::rpc::FindAddrReply;
using cfs::rpc::FindAddrRequest;
using cfs::rpc::GetSubDirPathsReply;
using cfs::rpc::GetSubDirPathsRequest;
using cfs::rpc::GetWriteAddrReply;
using cfs::rpc::GetWriteAddrRequest;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
// using cfs::rpc::HelloReply;
// using cfs::rpc::HelloRequest;
using cfs::FileMeta;
using cfs::rpc::MkdirReply;
using cfs::rpc::MkdirRequest;
using cfs::rpc::RCode;
using cfs::rpc::ReadChunkReply;
using cfs::rpc::ReadChunkRequest;
using cfs::rpc::RenameReply;
using cfs::rpc::RenameRequest;
using cfs::rpc::StatReply;
using cfs::rpc::StatRequest;
using cfs::rpc::WriteChunkReply;
using cfs::rpc::WriteChunkRequest;

void Client::Get(const std::string &remote_file_path,
                 const std::string &local_file_path) {
  FindAddrRequest req;
  FindAddrReply resp;
  req.set_file_path(remote_file_path);

  // rpc FindAddr()
  int master_nums = cli_master_stubs_.size();
  // assuming cli_master_stubs_[0] is leader.
  Status status;
  for (int i = master_nums - 1; i >= 0; --i) {
    ClientContext ctx;
    status = cli_master_stubs_[i]->FindAddr(&ctx, req, &resp);
  }
  if (status.ok()) {
    LOG(INFO) << "client_id: " << client_id_ << " FindAddr() done.";
  } else {
    // TODO: master failover.
    LOG(ERROR) << "client_id: " << client_id_
               << " FindAddr error: " << status.error_code() << ": "
               << status.error_message();
  }

  // vector is for testing convinience, can be omitted.
  std::vector<int64_t> handles;
  std::vector<std::vector<std::string>> ip_ports;
  int n = resp.chunk_handles_size();
  if (n <= 0) {
    LOG(ERROR) << "client_id: " << client_id_
               << " find no corresponding chunk handles when FindAddr()";
  }

  handles.reserve(n);
  ip_ports.reserve(n);
  LOG(INFO) << "FindAddr() res: ";
  for (int i = 0; i < n; ++i) {
    handles.push_back(resp.chunk_handles(i));
    LOG(INFO) << "handle_id: " << handles[i];

    std::vector<std::string> tmp;
    auto replica_addrs = resp.addrs(i);
    for (int j = 0; j < replica_addrs.ip_ports_size(); ++j) {
      std::string addr = replica_addrs.ip_ports(j);
      tmp.push_back(addr);
      LOG(INFO) << addr << ", ";
    }
    ip_ports.push_back(tmp);
  }

  size_t file_size = GetFileInfo(remote_file_path).second;
  LOG(INFO) << "remote_file_size: " << file_size;
  // std::cout << "remote_file_size: " << file_size << "\n";
  std::ofstream local_file;
  local_file.open(local_file_path);

  // read the all chunks from the chunk servers,
  // merge them into the local_file, the last chunk of data
  // requires special handling.

  std::string buf;
  int replica_size = ip_ports[0].size();
  // ReadFailover function.
  auto ReadFailover = [&](int handle_index, int read_length) {
    // failover
    int j = 0;
    for (; j < replica_size; ++j) {
      std::string read_server_addr = ip_ports[handle_index][j];
      bool success = false;
      // request CLINETRETRYTIMES times per chunkserver
      for (int k = 0; k < CLINETRETRYTIMES; ++k) {
        LOG(INFO) << k << "-th read " << read_server_addr;
        Status status = ReadChunk(read_server_addr, handles[handle_index], 0,
                                  read_length, &buf);
        if (status.ok()) {
          success = true;
          break;
        }
        LOG(INFO) << "Fail when Get():ReadChunk() " << status.error_code()
                  << ": " << status.error_message();
      }
      if (success) {
        break;
      }
    }
    if (j == replica_size) {
      LOG(ERROR) << "No available chunk server when read chunk-"
                 << handles[handle_index];
    }
  };

  for (int i = 0; i < n - 1; ++i) {
    ReadFailover(i, CHUNKSIZE);
    // write the data into the local file.
    local_file << buf;
  }

  int64_t last_chunk_read_length = file_size - (n - 1) * CHUNKSIZE;
  ReadFailover(n - 1, last_chunk_read_length);
  local_file << buf;
  // TODO: check the integrity during the transmission.
  local_file.close();
  LOG(INFO) << "Get() exit.";
}

void Client::Put(const std::string &remote_file_path,
                 const std::string &local_file_path) {
  GetWriteAddrRequest req;
  GetWriteAddrReply resp;

  req.set_remote_file_path(remote_file_path);
  size_t file_size = GetFileSize(local_file_path);
  req.set_write_data_size(file_size);

  // rpc GetWriteAddr()
  int master_nums = cli_master_stubs_.size();
  // assuming cli_master_stubs_[0] is leader.
  Status status;
  for (int i = master_nums - 1; i >= 0; --i) {
    ClientContext ctx;
    status = cli_master_stubs_[i]->GetWriteAddr(&ctx, req, &resp);
  }

  if (status.ok()) {
    LOG(INFO) << "Put():GetWriteAddr() Succeed.";
  } else {
    // TOOD: master failover
    LOG(ERROR) << "Put():GetWriteAddr() Failed." << status.error_code() << ": "
               << status.error_message();
  }

  // vector is for testing convinience, can be omitted.
  std::vector<int64_t> handles;
  std::vector<std::vector<std::string>> ip_ports;
  int n = resp.chunk_handles_size();
  if (n <= 0) {
    LOG(ERROR) << "client_id: " << client_id_
               << ", find no corresponding chunk handles when GetWriteAddr()";
  }
  handles.reserve(n);
  ip_ports.reserve(n);
  for (int i = 0; i < n; ++i) {
    handles.push_back(resp.chunk_handles(i));
    LOG(INFO) << "handle_id: " << handles[i];

    std::vector<std::string> tmp;
    auto replica_addrs = resp.addrs(i);
    for (int j = 0; j < replica_addrs.ip_ports_size(); ++j) {
      std::string addr = replica_addrs.ip_ports(j);
      tmp.push_back(addr);
      LOG(INFO) << addr << ", ";
    }
    ip_ports.push_back(tmp);
  }

  // now we know where (ip_ports and which chunk_handles) to write the data
  // write the local file to chunk servers,
  // the last chunk of data requires special handling.
  std::string buf;
  buf.resize(CHUNKSIZE);
  int replica_size = ip_ports[0].size();
  std::ifstream local_file(local_file_path, std::ifstream::binary);
  // WriteFailover function.
  auto WriteFailover = [&](int handle_index, int write_length) {
    // failover
    int j = 0;
    std::vector<std::string> data_locations = ip_ports[handle_index];
    for (; j < replica_size; ++j) {
      std::string write_server_addr = data_locations[j];
      bool success = false;
      // request CLINETRETRYTIMES times per chunkserver
      for (int k = 0; k < CLINETRETRYTIMES; ++k) {
        LOG(INFO) << k << "-th write " << write_server_addr;
        // write data into chunk
        // LOG(INFO) << "-------- in lambda write_failover(), before "
        //              "WriteChunk(...) -----";
        Status status = WriteChunk(write_server_addr, handles[handle_index], 0,
                                   write_length, data_locations, buf);
        // LOG(INFO) << "-------- in lambda write_failover(), after "
        //              "WriteChunk(...) -----";
        if (status.ok()) {
          success = true;
          break;
        }
        LOG(INFO) << "Fail when Put():WriteChunk() " << status.error_code()
                  << ": " << status.error_message();
      }
      if (success) {
        break;
      }
    }
    if (j == replica_size) {
      LOG(ERROR) << "No available chunk server when write chunk-"
                 << handles[handle_index];
    }
  };
  for (int i = 0; i < n - 1; ++i) {
    // from local_file read `CHUNKSIZE` bytes into `buf`.
    local_file.read(&buf[0], CHUNKSIZE);
    // write data into chunk.
    WriteFailover(i, CHUNKSIZE);
  }
  // last write
  int64_t last_chunk_write_length = file_size - (n - 1) * CHUNKSIZE;
  local_file.read(&buf[0], last_chunk_write_length);
  WriteFailover(n - 1, last_chunk_write_length);
  // WriteChunk(ip_ports[n - 1], handles[n - 1], 0, last_chunk_write_length,
  // buf);

  local_file.close();
  LOG(INFO) << "Put() exit.";
}

void Client::Stat(const std::string &file_name) {
  StatRequest req;
  StatReply resp;
  ClientContext ctx;
  Status status = cli_master_stubs_[0]->Stat(&ctx, req, &resp);
  if (resp.ret_code() == RCode::FILE_NOTFOUND) {
    std::cout << "Stat():" << file_name << " does not exist.\n";
  } else if (resp.ret_code() == RCode::OK) {
    FileMeta file_meta = resp.file_meta();
    std::string file_path = file_meta.file_path();
    uint64_t file_size = file_meta.file_size();
    std::cout << "file name: " << file_path << ", size: " << file_size << "\n";
  }
  // TODO: retcode -> other error
  std::cout << "Stat() exit.\n";
}

void Client::Rename(const std::string &src_path, const std::string &des_path) {
  RenameRequest req;
  RenameReply resp;
  req.set_src_path(src_path);
  req.set_des_path(des_path);

  int master_nums = cli_master_stubs_.size();
  // assuming cli_master_stubs_[0] is leader.
  Status status;
  for (int i = master_nums - 1; i >= 0; --i) {
    ClientContext ctx;
    status = cli_master_stubs_[i]->Rename(&ctx, req, &resp);
  }
  if (status.ok()) {
    LOG(INFO) << "Rename(): " << src_path << " renamed to " << des_path;
  } else {
    LOG(ERROR) << "Rename() Failed. " << status.error_code() << ": "
               << status.error_message();
  }
}

void Client::Delete(const std::string &remote_file_path) {
  DeleteRequest req;
  DeleteReply resp;
  req.set_remote_file_path(remote_file_path);

  int master_nums = cli_master_stubs_.size();
  // assuming cli_master_stubs_[0] is leader.
  Status status;
  for (int i = master_nums - 1; i >= 0; --i) {
    ClientContext ctx;
    status = cli_master_stubs_[i]->Delete(&ctx, req, &resp);
  }
  if (status.ok()) {
    LOG(INFO) << "Delete(): " << remote_file_path << " is deleted.";
    // std::cout << remote_file_path << "is deleted.\n";
  } else {
    LOG(ERROR) << "Delete() Failed. " << status.error_code() << ": "
               << status.error_message();
    std::cout << "Fail to delete" << remote_file_path << "\n";
  }
}

void Client::Mkdir(const std::string &new_file_path) {
  MkdirRequest req;
  MkdirReply resp;

  req.set_new_file_path(new_file_path);

  int master_nums = cli_master_stubs_.size();
  // assuming cli_master_stubs_[0] is leader.
  Status status;
  for (int i = master_nums - 1; i >= 0; --i) {
    ClientContext ctx;
    status = cli_master_stubs_[i]->Mkdir(&ctx, req, &resp);
  }
  if (status.ok()) {
    LOG(INFO) << "Mkdir(): " << new_file_path << " is created.";
  } else {
    LOG(ERROR) << "Mkdir() Failed. " << status.error_code() << ": "
               << status.error_message();
    std::cout << "Fail to create " << new_file_path << " directory.\n";
  }
}

void Client::List(const std::string &remote_file_path) {
  // 1. Get the sub-dir file names -> rpc GetSubDirPaths()
  // 2. Get every file path and size -> rpc Stat()
  GetSubDirPathsRequest req;
  GetSubDirPathsReply resp;
  req.set_file_path(remote_file_path);

  // rpc GetSubDirPaths() -> get the names of all files in the first-level
  // directory
  int master_nums = cli_master_stubs_.size();
  // assuming cli_master_stubs_[0] is leader.
  Status status;
  for (int i = master_nums - 1; i >= 0; --i) {
    ClientContext ctx;
    status = cli_master_stubs_[i]->GetSubDirPaths(&ctx, req, &resp);
  }
  if (status.ok()) {
    LOG(INFO) << "GetSubDirPaths() succeed";
  } else {
    LOG(ERROR) << "GetSubDirPaths() Failed. " << status.error_code() << ": "
               << status.error_message();
  }
  // infos_vec: stores the path and size of all files.
  int n = resp.subdir_file_paths_size();
  std::vector<std::pair<std::string, std::uint64_t>> infos_vec;
  for (int i = 0; i < n; ++i) {
    auto path_size_pa = GetFileInfo(resp.subdir_file_paths(i));
    infos_vec.push_back(path_size_pa);
    // for testing.
    std::cout << "file_name: " << path_size_pa.first
              << ", file_size: " << path_size_pa.second << "\n";
  }
  // TODO: error handling.
}

// ReadChunk():
// `addr`: chunk server's ip and port.
// `chunk_handle`: identifies the chunk to be read.
// `offset`: where to begin reading.
// `length`: length of data to be read.
// `res`: store all the data read out.
Status Client::ReadChunk(const std::string &addr, int64_t chunk_handle,
                         int64_t offset, size_t length, std::string *res) {
  // 1. find the cli_chunk_server_stub by addr;
  auto cli2chunkserver_stub = GetCliChunkServerStub(addr);

  // 2. rpc ReadChunk(): cli_chunk_server_stub(req, resp, ctx);
  ReadChunkRequest req;
  ReadChunkReply resp;
  ClientContext ctx;
  req.set_chunkhandle(chunk_handle);
  req.set_offset(offset);
  req.set_length(length);
  LOG(INFO) << "ReadChunk(): length is " << length;
  // cli -> chunkserver. addr, ip:port
  Status status = cli2chunkserver_stub->ReadChunk(&ctx, req, &resp);
  if (status.ok()) {
    if (resp.bytes_read() == 0) {
      return Status(grpc::NOT_FOUND, "Data not found at chunkserver.");
    } else if ((size_t)resp.bytes_read() != length) {
      LOG(INFO) << "Warning: ReadChunk read " << resp.bytes_read()
                << " bytes but asked for " << length << ".";
    }
    // everything is fine.
    // 3. parsing the data into res from responses;
    *res = resp.data();
    LOG(INFO) << "ReadChunk(): read data content: " << *res;
  } else {
    LOG(ERROR) << "ReadChunk() Failed." << status.error_code() << ": "
               << status.error_message();
  }

  return status;
}

// WriteChunk():
// `addr`: chunk server's ip and port.
// `chunk_handle`: identifies the chunk to be written.
// `offset`: where to begin writing.
// `length`: length of data to be written.
// `data`: the data to be written.
Status Client::WriteChunk(const std::string &addr, int64_t chunk_handle,
                          int64_t offset, size_t length,
                          const std::vector<std::string> &locations,
                          const std::string &data) {
  auto cli2chunkserver_stub = GetCliChunkServerStub(addr);
  WriteChunkRequest req;
  WriteChunkReply resp;
  ClientContext ctx;
  req.set_client_id(client_id_);
  req.set_chunkhandle(chunk_handle);
  req.set_offset(offset);
  req.set_length(length);
  req.set_data(data);
  for (auto addr : locations) {
    req.add_locations(addr);
  }
  // LOG(INFO) << "------------ in WriteChunk() -----------";
  Status status = cli2chunkserver_stub->WriteChunk(&ctx, req, &resp);
  if (status.ok()) {
    LOG(INFO) << "rpc WriteChunk() succeed";
  } else {
    LOG(ERROR) << "<remote addr: " << addr << ">"
               << "rpc WriteChunk() Failed." << status.error_code() << " : "
               << status.error_message();
  }
  LOG(INFO) << "------------ WriteChunk() exit -----------";
  return status;
}

ChunkServerService::Stub *Client::GetCliChunkServerStub(
    const std::string &addr) {
  // // std::map<std::string, std::unique_ptr<Cli2ChunkServer::Stub>>
  // // cli_chunkserver_stub_map_;
  // auto it = cli_chunkserver_stub_map_.find(addr);
  // if (it != cli_chunkserver_stub_map_.end()) {
  //   return it->second;
  // }
  // // If the corresponding stub does not exist, create one and record it into
  // // map.
  // auto new_stub = Cli2ChunkServer::NewStub(
  //     grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()));
  // cli_chunkserver_stub_map_[addr] = new_stub.get();
  // return cli_chunkserver_stub_map_[addr];

  ChunkServerService::Stub *result;
  grpc::ChannelArguments argument;
  argument.SetMaxReceiveMessageSize(100 * 1024 * 1024);
  auto it = cli_chunkserver_stub_map_.find(addr);
  if (it != cli_chunkserver_stub_map_.end()) {
    result = it->second.get();
  } else {
    auto stub = ChunkServerService::NewStub(grpc::CreateCustomChannel(
        addr, grpc::InsecureChannelCredentials(), argument));
    result = stub.get();
    cli_chunkserver_stub_map_[addr] = std::move(stub);
  }
  return result;
}

std::pair<std::string, std::uint64_t> Client::GetFileInfo(
    const std::string &filename) {
  StatRequest req;
  StatReply resp;
  req.set_filepath(filename);
  // rpc Stat()

  int master_nums = cli_master_stubs_.size();
  // assuming cli_master_stubs_[0] is leader.
  Status status;
  for (int i = master_nums - 1; i >= 0; --i) {
    ClientContext ctx;
    status = cli_master_stubs_[i]->Stat(&ctx, req, &resp);
  }
  if (status.ok()) {
    std::cout << "rpc Stat() succeed";
  } else {
    std::cout << "rpc Stat() failed. " << status.error_code() << ": "
              << status.error_message();
  }
  // message StatReply {
  //     RCode ret_code = 1;
  //     FileMeta file_meta = 2;
  // }
  if (resp.ret_code ==) {
    resp.set_file_name("");
  }
  cout << "GetFileInfo() exit.\n";
  return std::make_pair(resp.file_name(), resp.file_size());
}

std::pair<std::string, unsigned short> Client::ParseIpPort(
    const std::string &addr) {
  // TODO: check addr whether valid, like "127.0.0.1:1412".
  int i = 0;
  int n = addr.size();
  for (; i < n; ++i) {
    if (addr[i] == ':') {
      break;
    }
  }
  return std::make_pair(addr.substr(0, i),
                        std::stoi(addr.substr(i + 1, n - i)));
}

size_t Client::GetFileSize(const std::string &filename) {
  std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  // actual type: std::ifstream::pos_type
  size_t n = in.tellg();
  in.close();
  return n;
}

// test
void Client::TestCreate(const std::string remote_file_path) {
  GetWriteAddrRequest req;
  GetWriteAddrReply resp;
  req.set_remote_file_path(remote_file_path);
  // size_t file_size = GetFileSize(local_file_path);
  size_t file_size = 1024;
  req.set_write_data_size(file_size);

  int master_nums = cli_master_stubs_.size();
  // assuming cli_master_stubs_[0] is leader.
  Status status;
  for (int i = master_nums - 1; i >= 0; --i) {
    ClientContext ctx;
    status = cli_master_stubs_[i]->GetWriteAddr(&ctx, req, &resp);
  }
  if (status.ok()) {
    LOG(INFO) << "Put():GetWriteAddr() Succeed.";
  } else {
    LOG(ERROR) << "TestCreate() Failed. Error code: " << status.error_code()
               << ": " << status.error_message();
  }
}
void Client::TestWrite(const std::string remote_file_path) {
  std::vector<std::string> locations = {
      "47.97.218.168:50052", "47.97.218.168:50051", "47.97.218.168:50053"};
  WriteChunk("47.97.218.168:50052", 1, 0, 0, locations, "asdasdasdasda");
}
void Client::TestFindAddr(const std::string &file_path) {
  FindAddrRequest req;
  FindAddrReply reply;
  ClientContext ctx;
  req.set_file_path(file_path);
  Status status = cli_master_stubs_[0]->FindAddr(&ctx, req, &reply);
  for (int i = 0; i < reply.chunk_handles_size(); ++i) {
    std::cout << reply.chunk_handles(i) << ", ";
  }
  std::cout << "\n";
  for (int i = 0; i < reply.addrs_size(); ++i) {
    auto replicas_ip_ports = reply.addrs(i);
    for (int j = 0; j < replicas_ip_ports.ip_ports_size(); ++j) {
      auto ip_port = replicas_ip_ports.ip_ports(j);
      std::cout << ip_port << ",";
    }
    std::cout << "\n";
  }
  // for (auto replicas_ip_port : reply.addrs()) {
  //   for (auto ip_port : replicas_ip_port.ip_ports()) {
  //     std::cout << ip_port << ",";
  //   }
  //   std::cout << "\n";
  // }
  std::cout << "\n";
}

/*
void Client::TestHello(std::string name) {
  // auto stub = GetCliChunkServerStub("47.97.218.168:50051");
  std::string addr = "47.97.218.168:50051";
  grpc::ChannelArguments argument;
  argument.SetMaxReceiveMessageSize(100 * 1024 * 1024);
  auto stub = Cli2ChunkServer::NewStub(grpc::CreateCustomChannel(
      addr, grpc::InsecureChannelCredentials(), argument));

  HelloRequest req;
  req.set_name(name);
  HelloReply resp;
  ClientContext ctx;
  auto status = stub->SayHello(&ctx, req, &resp);
  if (status.ok()) {
    LOG(INFO) << "TestHello() Succeed. msg: " << resp.message();
  } else {
    LOG(ERROR) << "TestHello() Failed. Error code: " << status.error_code()
               << ": " << status.error_message();
  }
}
*/
