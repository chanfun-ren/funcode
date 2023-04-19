find_package(protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
find_package(Threads)

#
# Protobuf/Grpc source files
#
set(PROTO_FILES
    cfs.proto
    metadata.proto
)

#
# Add Library target with protobuf sources
#
add_library(myproto SHARED ${PROTO_FILES})
target_link_libraries(myproto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)
# link gRPC::grpc++_reflection ?

target_include_directories(myproto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(myproto PUBLIC ${GRPC_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIR})

message(STATUS "protos includes: ${GRPC_INCLUDE_DIR}")


#
# Compile protobuf and grpc files in myproto target to cpp
#
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET myproto LANGUAGE cpp)
protobuf_generate(TARGET myproto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
