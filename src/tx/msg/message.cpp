//
// Created by focus on 2/10/24.
//

#include "message.h"

#include <ostream>
#include <tuple>

#include <common/log/log.h>
#include <tx/msg/generator_msg_id.h>

namespace sdb::tx::msg {

std::ostream& operator<<(std::ostream& stream, const MessageType type) {
	switch (type) {
		case MessageType::MSG_UNDEFINED:
			stream << "MSG_UNDEFINED";
			break;
		case MessageType::MSG_START:
			stream << "MSG_START";
			break;
		case MessageType::MSG_START_ACK:
			stream << "MSG_START_ACK";
			break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Message& msg) {
	stream << "message "
		   << "[type=" << msg.type << "]"
		   << "[id=" << msg.msg_id << "]"
		   	<< "[from " << msg.source << "]"
		   	<< "[to " << msg.destination << "]"
			   << " todo payload";
	return stream;
}

bool operator==(const Message& lhs, const Message& rhs) {
	return std::make_tuple(lhs.msg_id, lhs.type, lhs.source, lhs.destination)
		==
		std::make_tuple(rhs.msg_id, rhs.type, rhs.source, rhs.destination);
}

bool operator<(const Message& lhs, const Message& rhs) {
	return std::make_tuple(lhs.type, lhs.msg_id, lhs.source, lhs.destination)
		   <
		   std::make_tuple(rhs.type, rhs.msg_id, rhs.source, rhs.destination);
}

Message CreateMsgStartAck(const ActorID source,
						  const ActorID destination,
						  const TxID txid,
						  const Timestamp read_ts) {
	Message msg;
	msg.source = source;
	msg.destination = destination;
	msg.type = MessageType::MSG_START_ACK;

	msg.msg_id = GeneratorMsgID::get_next_id();

	MsgAckStartPayload payload{
		.txid=txid,
		.read_ts=read_ts
	};

	msg.payload.payload = payload;

	LOG_DEBUG << "CreateMsgStartAck: " << msg;

	return msg;
}

} // namespace sdb::tx::msg