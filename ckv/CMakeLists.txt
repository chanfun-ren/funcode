cmake_minimum_required(VERSION 3.14)
project(ckv VERSION 0.1.0)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(test)


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

# 添加可执行文件 opt
add_executable(opt)
target_sources(opt
  PUBLIC
    src/cxxopt.cc
)
target_include_directories(opt
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
