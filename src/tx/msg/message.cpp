//
// Created by focus on 2/10/24.
//

#include "message.h"

#include <ostream>
#include <tuple>

#include <common/log/log.h>
#include <tx/generator/generator.h>

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
		   	<< "[source " << msg.source << "]"
		   	<< "[destination " << msg.destination << "]"
			<< " with payload: " << msg.payload;
	return stream;
}

bool operator==(const Message& lhs, const Message& rhs) {
	return std::make_tuple(lhs.msg_id, lhs.type, lhs.source, lhs.destination, lhs.payload)
		==
		std::make_tuple(rhs.msg_id, rhs.type, rhs.source, rhs.destination, rhs.payload);
}

bool operator<(const Message& lhs, const Message& rhs) {
	return std::make_tuple(lhs.type, lhs.msg_id, lhs.source, lhs.destination)
		   <
		   std::make_tuple(rhs.type, rhs.msg_id, rhs.source, rhs.destination);
}

TxID get_txid_from_msg_payload(const msg::Message& msg) {
	switch (msg.type) {
		case msg::MessageType::MSG_UNDEFINED:
			return UNDEFINED_TX_ID;
		case msg::MessageType::MSG_START:
			return msg.payload.get<msg::MsgStartPayload>().txid;
		case msg::MessageType::MSG_START_ACK:
			return msg.payload.get<msg::MsgAckStartPayload>().txid;
	}
	throw std::runtime_error("switch doesn't handle all cases.");
}

Message CreateMsgStart(const ActorID source, const ActorID destination) {
	Message msg;
	msg.type = MessageType::MSG_START;
	msg.source = source;
	msg.destination = destination;
	msg.msg_id = Generator::get_next_msg_id();

	MsgStartPayload payload{
		.txid=Generator::get_next_tx_id(),
		.read_ts=UNDEFINED_TS
	};

	msg.payload.set(payload);

	return msg;
}

Message CreateMsgStartAck(const ActorID source,
						  const ActorID destination,
						  const TxID txid,
						  const Timestamp read_ts) {
	Message msg;
	msg.source = source;
	msg.destination = destination;
	msg.type = MessageType::MSG_START_ACK;

	msg.msg_id = Generator::get_next_msg_id();

	MsgAckStartPayload payload{
		.txid=txid,
		.read_ts=read_ts
	};

	msg.payload.payload = payload;

	LOG_DEBUG << "CreateMsgStartAck: " << msg;

	return msg;
}

} // namespace sdb::tx::msg