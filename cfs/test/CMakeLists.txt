cmake_minimum_required(VERSION 3.13)
find_package(Catch2 3 REQUIRED)

# These tests can use the Catch2-provided main
add_executable(file_tree_test file_tree_test.cc)
target_link_libraries(file_tree_test PRIVATE Catch2::Catch2WithMain file_tree)




# # These tests need their own main
# add_executable(custom-main-tests test.cpp test-main.cpp)
# target_link_libraries(custom-main-tests PRIVATE Catch2::Catch2)