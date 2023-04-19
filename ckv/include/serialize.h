#pragma once

#include "protocol.h"

namespace ckv {

//------------------- kv_store log usage -------------------------/
std::string serialize(Command cmd);
Command deserialize(std::string line);
//----------------------------------------------------------------/

//------------------- Command network usag------------------------/
// INVALID command should not be serialized/deserialized
int serialize(const Command& cmd, std::string& out);
int deserialize(const std::string& in, Command& cmd);
//----------------------------------------------------------------/

//------------------- Response network usag------------------------/
int serialize(const Response& resp, std::string& str);
int deserialize(const std::string& str, Response& resp);
//----------------------------------------------------------------/

}  // namespace ckv
