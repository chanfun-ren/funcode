cmake_minimum_required(VERSION 3.14)
project(ckv VERSION 0.1.0)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(test)


add_library(common)
target_sources(common
  PRIVATE
    src/serialize.cc
)
target_include_directories(common
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)


add_executable(ckv)
target_sources(ckv
  PRIVATE
    src/main.cc
    src/kv_store.cc
)
target_include_directories(ckv
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(ckv common)

# 添加可执行文件 ckv
add_executable(ckv-server)
target_sources(ckv-server
  PRIVATE
    src/server_main.cc
    src/kv_store.cc
)
target_include_directories(ckv-server
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(ckv-server common)

# 添加可执行文件 ckv-cli
add_executable(ckv-cli)
target_sources( ckv-cli
  PRIVATE
    src/client_main.cc
)
target_include_directories(ckv-cli
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(ckv-cli common)
