#include "kv_store.h"

#include "serialize.h"

namespace ckv {
KvStore::KvStore(std::string_view path)
    : path_(std::string(path)), uncompacted_size_(0) {
  Status status = Open();
  if (!status.isOk()) {
    std::cerr << "Database initialization failed" << std::endl;
    return;
  }
  std::cout << "Database initialized successfully" << std::endl;
}
KvStore::~KvStore() {
  log_reader_.close();
  log_writer_.close();
}

std::string KvStore::GetDBPath() { return path_; }

Status KvStore::Set(const std::string& key, const std::string& value) {
  // Write log first
  Command cmd = {CommandType::SET, key, value};
  std::string command_log_entry = serialize(cmd);
  std::streampos pos = log_writer_.tellp();
  log_writer_ << command_log_entry;
  log_writer_.flush();
  if (log_writer_.bad()) {
    log_writer_.seekp(pos);
    return Status::IOError("Failed to write to log file");
  }

  // 更新索引
  index_[key] =
      LogEntryPos{pos, static_cast<std::size_t>(command_log_entry.size())};

  uncompacted_size_ += command_log_entry.size();
  return MaybeCompact();
  // return Status::Ok();
}

Status KvStore::Get(const std::string& key, std::string& value) {
  auto it = index_.find(key);
  if (it == index_.end()) {
    return Status::NotFound("Key not found");
  }

  std::size_t offset = it->second.offset;
  std::size_t len = it->second.len;

  // size_t offset -> long unsigned int
  // std::streampos -> long int
  // TODO:隐患
  log_reader_.seekg(offset, std::ios::beg);
  std::string line;
  std::getline(log_reader_, line);  // 每个 log_entry 占一行
  log_reader_.clear();
  // deserialize -> parse cmd
  Command cmd = deserialize(line);
  if (cmd.type != CommandType::SET) {
    // std::cerr << "get " << key << ": offset: " << offset << std::endl;
    return Status::Corruption("Log entry is corruption.");
  }
  // `set` command log entry
  value = cmd.value;

  return Status::Ok();
}

Status KvStore::Remove(const std::string& key) {
  auto it = index_.find(key);
  if (it == index_.end()) {
    return Status::NotFound("Key not found");
  }

  // Write log first
  Command cmd = {CommandType::RM, key, ""};
  std::string command_log_entry = serialize(cmd);

  std::streampos pos = log_writer_.tellp();
  log_writer_ << command_log_entry;
  log_writer_.flush();
  if (log_writer_.bad()) {
    log_writer_.seekp(pos);
    return Status::IOError("Failed to write to log file");
  }

  // 更新索引，删除 key
  index_.erase(it);
  return Status::Ok();
}

Status KvStore::Open() {
  // Initialize KvStore and open/create the file based on the path, and
  // initialize log_writer_ and log_reader_.

  // Try to create the file
  std::string path = this->GetDBPath();
  if (!std::filesystem::exists(path)) {
    std::ofstream file(path);  // create and open
    if (!file.is_open()) {
      return Status::IOError("Failed to create file.");
    }
    file.close();
  }

  // Open the file and initialize log_writer_ and log_reader_.
  std::ofstream writer(path, std::ios::app);  // seek to end.
  if (!writer.is_open()) {
    return Status::IOError("Failed to open file for writing");
  }
  log_writer_ = std::move(writer);

  std::ifstream reader(path);
  if (!reader.is_open()) {
    return Status::IOError("Failed to open file for reading");
  }

  // Replay the log and write to index_.
  std::string line;
  while (std::getline(reader, line)) {
    // update uncompacted_size_
    uncompacted_size_ += line.size() + 1;
    Command cmd = deserialize(line);
    switch (cmd.type) {
      case CommandType::RM: {
        // `rm` command log entry
        std::string key = cmd.key;
        index_.erase(key);
        break;
      }
      case CommandType::SET: {
        // `set` command log entry
        std::string key = cmd.key;
        std::string value = cmd.value;
        // update log entry offset
        std::size_t len = line.size() + 1;
        index_[key] =
            LogEntryPos{static_cast<std::size_t>(reader.tellg()) - len, len};
        break;
      }
      default: {
        return Status::Corruption("Log entry is corruption");
        break;
      }
    }
  }
  // print_index();

  reader.clear();
  reader.seekg(0, std::ios::beg);
  log_reader_ = std::move(reader);

  return Status::Ok();
}

Status KvStore::MaybeCompact() {
  if (uncompacted_size_ <= COMPACTION_THRESHOLD) {
    return Status::Ok();
  }
  std::cout << "uncompacted_size_: " << uncompacted_size_ << "\n";

  // Scan log file and record the latest log entry for each key
  // use index_ directly?
  std::unordered_map<std::string, LogEntryPos> latest_entries;
  log_reader_.clear();
  log_reader_.seekg(0, log_reader_.beg);
  std::string line;
  while (std::getline(log_reader_, line)) {
    Command command = deserialize(line);
    if (command.type == CommandType::SET) {
      std::size_t len = line.size() + 1;
      latest_entries[command.key] = {log_reader_.tellg() - len, len};
    } else if (command.type == CommandType::RM) {
      latest_entries.erase(command.key);
    } else if (command.type == CommandType::INVALID) {
      return Status::Corruption("Log entry is corruption");
    }
  }
  log_reader_.clear();
  log_reader_.seekg(0, log_reader_.beg);

  // Create a new log file and write all non-expired entries to it
  std::string new_log_file = path_ + "." + std::to_string(time(nullptr));
  std::ofstream new_log_writer(new_log_file, std::ios::binary);
  for (const auto& [key, entry] : latest_entries) {
    std::string value;
    if (Get(key, value) != Status::Ok()) {
      // If Get operation fails, return the error
      new_log_writer.close();
      std::filesystem::remove(new_log_file);
      return Status::Corruption("Corruption when compacting");
    }
    Command command{CommandType::SET, key, value};
    std::string log_entry = serialize(command);
    new_log_writer << log_entry;
    if (new_log_writer.bad()) {
      // If write to new log file fails, close and remove the file, and return
      // the error
      new_log_writer.close();
      std::filesystem::remove(new_log_file);
      return Status::IOError("Failed to write to new log file");
    }
  }
  // Flush the output stream to ensure all data is written to disk
  new_log_writer.flush();
  if (new_log_writer.bad()) {
    new_log_writer.close();
    std::filesystem::remove(new_log_file);
    return Status::IOError("Failed to flush new log file");
  }
  new_log_writer.close();

  // Update KvStore's members to use the new log file
  path_ = new_log_file;
  log_writer_.close();
  log_reader_.close();
  log_writer_.open(path_, std::ios::app);
  log_reader_.open(path_, std::ios::in);
  uncompacted_size_ = 0;
  std::cout << "compacting done.\n";
  // std::filesystem::remove(path_);
  return Status::Ok();
}

void KvStore::print_index() {
  for (auto& [k, pos] : index_) {
    std::cout << k << ": offset: " << pos.offset << " len: " << pos.len << "\n";
  }
  std::cout << "\n";
}

void KvStore::test_reader() {
  std::string line;
  while (std::getline(log_reader_, line)) {
    printf("%s\n", line.c_str());
  }
  log_reader_.clear();
  log_reader_.seekg(0);
}
}  // namespace ckv
