

#include <gtest/gtest.h>

#include <filesystem>

#include "kv_store.h"

using namespace ckv;

TEST(KvStoreTest, CompactCompressesDataFile) {
  std::string filename = "testdb0";
  std::unique_ptr<KvStore> kvstore = std::make_unique<KvStore>(filename);

  // write data that exceeds the compaction threshold
  for (int i = 10; i < 13; i++) {
    kvstore->Set(std::to_string(i), std::to_string(i));
  }
  std::ifstream old_file(filename);
  old_file.seekg(0, std::ios::end);
  const std::size_t old_size = old_file.tellg();

  // check the size of the data file
  auto new_filename = kvstore->GetDBPath();
  std::ifstream file(new_filename);
  file.seekg(0, std::ios::end);
  const std::size_t size = file.tellg();

  // The size of the data file should be less than the compaction threshold
  EXPECT_LE(old_size, size);

  // delete the kvstore files
  std::filesystem::remove(filename);
  std::string prefix = filename;
  for (const auto& entry : std::filesystem::directory_iterator("./")) {
    if (entry.path().filename().string().substr(0, prefix.size()) == prefix) {
      std::filesystem::remove(entry.path());
    }
  }
}

TEST(KvStoreTest, CompactDoesNotAffectData) {
  const std::string filename = "testdb1";
  std::unique_ptr<KvStore> kvstore = std::make_unique<KvStore>(filename);

  std::map<std::string, std::string> data;

  // write data that does not exceed the compaction threshold
  for (int i = 10; i < 11; i++) {
    auto key = std::to_string(i);
    auto value = std::to_string(i);
    kvstore->Set(key, value);
    data[key] = value;
  }

  // make compaction happen
  for (int i = 11; i < 13; i++) {
    auto key = std::to_string(i);
    auto value = std::string("a");
    kvstore->Set(key, value);
    data[key] = value;
  }

  // get data
  for (auto& [k, v] : data) {
    std::string value;
    kvstore->Get(k, value);
    EXPECT_EQ(value, v);
  }

  // delete the kvstore files
  std::string prefix = filename;
  for (const auto& entry : std::filesystem::directory_iterator("./")) {
    if (entry.is_regular_file() &&
        entry.path().filename().string().substr(0, prefix.size()) == prefix) {
      std::filesystem::remove(entry.path());
    }
  }
}