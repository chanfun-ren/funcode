
find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(spdlog REQUIRED)
find_package(Threads)

# Define the  target
message(STATUS "master_server include path: ${GRPC_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIR}")
include_directories(${PROJECT_BINARY_DIR})

# add_library(metadata_manager src/metadata_manager.cc src/file_tree.cc)
# target_link_libraries(metadata_manager  myproto) # 成功
# # target_link_options()

# add_library(master_service  src/master_service.cc)
# target_link_libraries(master_service metadata_manager)

set(CMAKE_BUILD_TYPE Debug)


add_library(file_tree src/file_tree.cc)
target_include_directories(file_tree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(file_tree PUBLIC spdlog::spdlog)
message(STATUS "master_server-CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(master_service src/master_service.cc src/metadata_manager.cc)
target_link_libraries(master_service myproto file_tree)

add_library(chunk_server_manager_service src/chunk_server_manager.cc)
target_link_libraries(chunk_server_manager_service myproto)

add_executable(server src/master_main.cc)


target_link_libraries(server
    master_service
    chunk_server_manager_service
    gRPC::grpc++_reflection 
    absl::flags
    absl::flags_parse)
    # ${_REFLECTION} # link gRPC::grpc++_reflection 
    # ${_GRPC_GRPCPP} # gRPC::grpc
    # ${_PROTOBUF_LIBPROTOBUF}) # protobuf::libprotobuf