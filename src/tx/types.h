#pragma once

#include <cstdint>
#include <vector>

namespace sdb::tx {

using ActorID = int64_t;
using MsgID = uint32_t;

namespace msg {
	struct Message;
}
using Messages = std::vector<msg::Message>;

// All events have unique timestamp in the system.
using Timestamp = int64_t;

using Key = int64_t;
using Value = int64_t;

// Unique ID of transaction.
using TxID = int64_t;

constexpr TxID UNDEFINED_TX_ID = -1;
constexpr TxID UNDEFINED_ACTOR_ID = -1;
constexpr Timestamp UNDEFINED_TS = -1;

// [start, end)
struct KeyInterval final {
	Key start;
	Key end;
};

using KeyIntervals = std::vector<KeyInterval>;

} // namespace sdb::tx