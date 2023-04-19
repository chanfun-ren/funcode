#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

namespace ckv {
enum StatusCode {
  kOk = 0,
  kNotFound = 1,
  kCorruption = 2,
  kNotSupported = 3,
  kInvalidArgument = 4,
  kIOError = 5,
  kNetworkError = 6
};

class Status {
 public:
  Status() : code_(kOk) {}

  Status(StatusCode code, const std::string& msg) : code_(code), msg_(msg) {}

  bool isOk() const { return code_ == kOk; }

  bool isNotFound() const { return code_ == kNotFound; }

  // or DataFormatError
  bool isCorruption() const { return code_ == kCorruption; }

  bool isNotSupported() const { return code_ == kNotSupported; }

  bool isInvalidArgument() const { return code_ == kInvalidArgument; }

  bool isIOError() const { return code_ == kIOError; }

  bool isNetworkError() const { return code_ == kNetworkError; }

  StatusCode code() const { return code_; }

  std::string message() const { return msg_; }

  static Status Ok() { return Status(); }

  static Status NotFound(const std::string& msg) {
    return Status(kNotFound, msg);
  }

  static Status Corruption(const std::string& msg) {
    return Status(kCorruption, msg);
  }

  static Status NotSupported(const std::string& msg) {
    return Status(kNotSupported, msg);
  }

  static Status InvalidArgument(const std::string& msg) {
    return Status(kInvalidArgument, msg);
  }

  static Status IOError(const std::string& msg) {
    return Status(kIOError, msg);
  }

  static Status NetworkError(const std::string& msg) {
    return Status(kNetworkError, msg);
  }

  bool operator==(const Status& other) const {
    return code_ == other.code_ && msg_ == other.msg_;
  }

  bool operator!=(const Status& other) const { return !(*this == other); }

 private:
  StatusCode code_;
  std::string msg_;
};

}  // namespace ckv
