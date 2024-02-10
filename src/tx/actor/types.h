#pragma once

#include <cstdint>
#include <vector>

namespace sdb::actor {

using ActorID = uint32_t;
using MsgID = uint32_t;

struct Message;
using Messages = std::vector<Message>;

using Timestamp = uint64_t;

} // namespace sdb::actor