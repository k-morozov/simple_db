#pragma once

#include <cstdint>
#include <vector>

namespace sdb::tx {

using ActorID = uint32_t;
using MsgID = uint32_t;

struct Message;
using Messages = std::vector<Message>;

// All events have unique timestamp in the system.
using Timestamp = uint64_t;

using Key = int64_t;

// Unique ID of transaction.
using TxID = int64_t;

constexpr TxID UNDEFINED_TX_ID = -1;

} // namespace sdb::tx