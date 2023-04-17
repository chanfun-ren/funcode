cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()


add_executable(
  mem_kv_test
  mem_kv_test.cc
  ${PROJECT_SOURCE_DIR}/src/kv_store.cc
)
target_include_directories(mem_kv_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(
  mem_kv_test
  GTest::gtest_main
)


add_executable(
  wal_mem_index_test
  wal_mem_index_test.cc
  ${PROJECT_SOURCE_DIR}/src/kv_store.cc
)
target_include_directories(wal_mem_index_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(
  wal_mem_index_test
  GTest::gtest_main
)


add_executable(
  compact_test
  compact_test.cc
  ${PROJECT_SOURCE_DIR}/src/kv_store.cc
)
target_include_directories(compact_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(
  compact_test
  GTest::gtest_main
)


include(GoogleTest)
gtest_discover_tests(mem_kv_test wal_mem_index compact_test)