#!/bin/sh
proto_dir="../protos"

proto_files=$(ls $proto_dir)

for proto_file in $proto_files
do
  protoc --cpp_out=../gen_code -I ../protos/ $proto_file
  # protoc --grpc_out=../gen_code --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` -I../protos/ $proto_file
  protoc --grpc_out=../gen_code --plugin=protoc-gen-grpc=/root/grpc_install_pos/bin/grpc_cpp_plugin -I../protos/ $proto_file
done