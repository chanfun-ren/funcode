#pragma once
#include <arpa/inet.h>   // for inet_pton
#include <netinet/in.h>  // for struct sockaddr_in

#include <regex>
#include <sstream>
#include <vector>

bool is_valid_ip(const std::string& str) {
  struct sockaddr_in sa;
  return inet_pton(AF_INET, str.c_str(), &(sa.sin_addr)) != 0;
}

bool is_valid_addr(const std::string& str) {
  std::vector<std::string> parts;
  std::istringstream iss(str);
  std::string part;
  while (std::getline(iss, part, ':')) {
    parts.push_back(part);
  }
  if (parts.size() != 2) {
    return false;
  }
  int ip_a, ip_b, ip_c, ip_d, port;
  if (std::sscanf(parts[0].c_str(), "%d.%d.%d.%d", &ip_a, &ip_b, &ip_c,
                  &ip_d) != 4 ||
      ip_a < 0 || ip_a > 255 || ip_b < 0 || ip_b > 255 || ip_c < 0 ||
      ip_c > 255 || ip_d < 0 || ip_d > 255 ||
      std::sscanf(parts[1].c_str(), "%d", &port) != 1 || port < 0 ||
      port > 65535) {
    return false;
  }
  return true;
}

bool IsValidAddress(const std::string& str) {
  std::regex pattern(R"((\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}):(\d+))",
                     std::regex_constants::ECMAScript);
  std::smatch matches;
  return std::regex_match(str, matches, pattern) &&
         std::stoi(matches[1]) <= 255 && std::stoi(matches[2]) <= 65535;
}
