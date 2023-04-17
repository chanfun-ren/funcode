namespace ckv {
std::string serialize(Command cmd) {
  std::string log_entry;
  switch (cmd.type) {
    case CommandType::SET: {
      log_entry = cmd.key + " " + cmd.value + "\n";
      break;
    }
    case CommandType::RM: {
      std::string empty_value = "";
      log_entry = cmd.key + " " + empty_value + "\n";
      break;
    }
  }
  return log_entry;
}

Command deserialize(std::string line) {
  auto pos = line.find(' ');
  if (pos == std::string::npos) {
    return Command{CommandType::INVALID, "", ""};
  }
  if (pos == line.size() - 1) {
    // `rm` command log entry
    std::string key = line.substr(0, pos);
    return Command{CommandType::RM, key, ""};
  } else {
    // `set` command log entry
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    return Command{CommandType::SET, key, value};
  }
}
}  // namespace ckv
