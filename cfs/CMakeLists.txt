cmake_minimum_required(VERSION 3.13)
project(cfs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "/root/grpc_install_pos/")




# Add the subdirectories
# add_subdirectory(common)
add_subdirectory(master_server)
add_subdirectory(protos)
add_subdirectory(chunk_server)
add_subdirectory(client)
add_subdirectory(test)

