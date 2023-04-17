#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "kv_store.h"

std::string read_file_to_string(std::string path) {
  std::ifstream t(path);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

TEST(KvStoreTest, BasicTest) {
  std::string db_path = "./test/test_db1";

  using namespace ckv;
  KvStore kv_store(db_path);

  // Test set() and get()
  ASSERT_EQ(kv_store.Set("foo", "bar"), Status::Ok());
  std::cout << read_file_to_string(db_path) << "--------\n";
  ASSERT_EQ(std::string("foo bar\n"), read_file_to_string(db_path));

  std::string value;
  ASSERT_EQ(kv_store.Get("foo", value), Status::Ok());
  ASSERT_EQ(value, "bar");
  ASSERT_EQ(std::string("foo bar\n"), read_file_to_string(db_path));

  // Test remove() and get()
  ASSERT_EQ(kv_store.Remove("foo"), Status::Ok());
  ASSERT_EQ(std::string("foo bar\nfoo \n"), read_file_to_string(db_path));

  ASSERT_EQ(kv_store.Get("foo", value), Status::NotFound("Key not found"));
  ASSERT_EQ(std::string("foo bar\nfoo \n"), read_file_to_string(db_path));
  std::ofstream file(db_path, std::ofstream::out | std::ofstream::trunc);
}