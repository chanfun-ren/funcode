#include "serialize.h"

#include <gtest/gtest.h>

#include <iostream>

TEST(Serialization, Command_Set) {
  ckv::Command cmd = {ckv::CommandType::SET, "key", "value"};

  std::string serialized_data;
  EXPECT_EQ(0, ckv::serialize(cmd, serialized_data));

  EXPECT_EQ(serialized_data, "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n");
  // if (serialized_data == "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n") {
  //   std::cout << serialized_data << "\n";
  // }

  ckv::Command deserialized_cmd;
  EXPECT_EQ(0, ckv::deserialize(serialized_data, deserialized_cmd));
  EXPECT_EQ(cmd.type, deserialized_cmd.type);
  EXPECT_EQ(cmd.key, deserialized_cmd.key);
  EXPECT_EQ(cmd.value, deserialized_cmd.value);
}

TEST(Serialization, Command_Get) {
  ckv::Command cmd = {ckv::CommandType::GET, "key", ""};

  std::string serialized_data;
  EXPECT_EQ(0, ckv::serialize(cmd, serialized_data));

  ckv::Command deserialized_cmd;
  EXPECT_EQ(0, ckv::deserialize(serialized_data, deserialized_cmd));
  EXPECT_EQ(cmd.type, deserialized_cmd.type);
  EXPECT_EQ(cmd.key, deserialized_cmd.key);
  EXPECT_EQ(cmd.value, deserialized_cmd.value);
}

TEST(Serialization, Command_Rm) {
  ckv::Command cmd = {ckv::CommandType::RM, "key", ""};

  std::string serialized_data;
  EXPECT_EQ(0, ckv::serialize(cmd, serialized_data));

  ckv::Command deserialized_cmd;
  EXPECT_EQ(0, ckv::deserialize(serialized_data, deserialized_cmd));
  EXPECT_EQ(cmd.type, deserialized_cmd.type);
  EXPECT_EQ(cmd.key, deserialized_cmd.key);
  EXPECT_EQ(cmd.value, deserialized_cmd.value);
}

TEST(Serialization, Response) {
  ckv::Response resp = {ckv::ResponseType::STATUS, "OK"};

  std::string serialized_data;
  EXPECT_EQ(0, ckv::serialize(resp, serialized_data));

  ckv::Response deserialized_resp;
  EXPECT_EQ(0, ckv::deserialize(serialized_data, deserialized_resp));
  EXPECT_EQ(resp.type, deserialized_resp.type);
  EXPECT_EQ(resp.message, deserialized_resp.message);
}