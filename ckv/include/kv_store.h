#pragma once
#include <filesystem>
#include <unordered_map>

#include "command.h"
#include "status.h"

// Define struct to represent a log entry pos
namespace ckv {

// constexpr std::size_t COMPACTION_THRESHOLD = 1024 * 1024;  // 1MB
constexpr std::size_t COMPACTION_THRESHOLD = 10;  // 10 bytes

struct LogEntryPos {
  std::size_t offset;
  std::size_t len;
};

class KvStore {
 private:
  std::unordered_map<std::string, LogEntryPos> index_;
  std::string path_;
  std::ofstream log_writer_;
  std::ifstream log_reader_;
  std::size_t uncompacted_size_;

 public:
  explicit KvStore(std::string_view path);
  ~KvStore();

  std::string GetDBPath();

  Status Set(const std::string& key, const std::string& value);

  Status Get(const std::string& key, std::string& value);

  Status Remove(const std::string& key);

  Status Open();

 private:
  Status MaybeCompact();  // TODO: make it private

 private:
  void print_index();

  void test_reader();
};
}  // namespace ckv
