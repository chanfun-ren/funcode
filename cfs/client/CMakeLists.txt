find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(spdlog REQUIRED)


# Define the  target
add_executable(client src/client_main.cc src/client.cc)


target_link_libraries(client
    spdlog::spdlog
    myproto
    absl::flags
    absl::flags_parse)
    # ${_REFLECTION} # link gRPC::grpc++_reflection 
    # ${_GRPC_GRPCPP} # gRPC::grpc
    # ${_PROTOBUF_LIBPROTOBUF}) # protobuf::libprotobuf
