//
// Created by focus on 2/10/24.
//

#pragma once

#include <cstdint>
#include <string>

#include <tx/types.h>

namespace sdb::tx {

enum class MessageType: uint8_t {
	MSG_UNDEFINED,
	MSG_START,
};

std::ostream& operator<<(std::ostream& stream, MessageType type);

struct Message {
	MessageType type{MessageType::MSG_UNDEFINED};
	ActorID source;
	ActorID destination;

	MsgID id;

	// @todo payload
};

} // namespace sdb::tx