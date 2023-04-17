#include <gtest/gtest.h>

#include <string>

#include "kv_store.h"

TEST(KvStoreTest, BasicTest) {
  std::string db_path = "./test/test_db0";
  using namespace ckv;
  KvStore kv_store(db_path);
  // Test set() and get()
  ASSERT_EQ(kv_store.Set("foo", "bar"), Status::Ok());
  std::string value;
  ASSERT_EQ(kv_store.Get("foo", value), Status::Ok());
  ASSERT_EQ(value, "bar");

  // Test remove() and get()
  ASSERT_EQ(kv_store.Remove("foo"), Status::Ok());
  ASSERT_EQ(kv_store.Get("foo", value), Status::NotFound("Key not found"));
}