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
		case MessageType::MSG_PUT:
			stream << "MSG_PUT";
			break;
		case MessageType::MSG_PUT_REPLY:
			stream << "MSG_PUT_REPLY";
			break;
		case MessageType::MSG_COMMIT:
			stream << "MSG_COMMIT";
			break;
		case MessageType::MSG_COMMIT_ACK:
			stream << "MSG_COMMIT_ACK";
			break;
		case MessageType::MSG_ROLLED_BACK_BY_SERVER:
			stream << "MSG_ROLLED_BACK_BY_SERVER";
			break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Message& msg) {
	stream << "message "
		   	<< "[type=" << msg.type << "]"
		   	<< "[msg_id=" << msg.msg_id << "]"
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
		case msg::MessageType::MSG_PUT:
			return msg.payload.get<msg::MsgPutPayload>().txid;
		case msg::MessageType::MSG_PUT_REPLY:
			return msg.payload.get<msg::MsgPutReplyPayload>().txid;
		case msg::MessageType::MSG_COMMIT:
			return msg.payload.get<msg::MsgCommitPayload>().txid;
		case msg::MessageType::MSG_COMMIT_ACK:
			return msg.payload.get<msg::MsgCommitAckPayload>().txid;
		case msg::MessageType::MSG_ROLLED_BACK_BY_SERVER:
			return msg.payload.get<msg::MsgRolledBackByServerPayload>().txid;
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

Message CreateMsgPut(ActorID source, ActorID destination, TxID txid, Key key, Value value) {
	Message msg;
	msg.type = MessageType::MSG_PUT;
	msg.source = source;
	msg.destination = destination;

	msg.msg_id = Generator::get_next_msg_id();

	MsgPutPayload payload{
		.txid=txid,
		.key=key,
		.value=value
	};

	msg.payload.payload = payload;

	return msg;
}

Message CreateMsgPutReply(ActorID source, ActorID destination, TxID txid, MsgID msg_id) {
	Message msg;
	msg.type = MessageType::MSG_PUT_REPLY;
	msg.source = source;
	msg.destination = destination;

	msg.msg_id = Generator::get_next_msg_id();

	MsgPutReplyPayload payload{
			.txid=txid,
			.msg_id=msg_id
	};

	msg.payload.payload = payload;

	return msg;
}

Message CreateMsgCommit(ActorID source, ActorID destination, TxID txid) {
	Message msg;
	msg.type = MessageType::MSG_COMMIT;
	msg.source = source;
	msg.destination = destination;

	msg.msg_id = Generator::get_next_msg_id();

	MsgCommitPayload payload{
			.txid=txid
	};

	msg.payload.payload = payload;

	return msg;
}

Message CreateMsgCommitAck(const ActorID source, const ActorID destination, const TxID txid, const Timestamp commit_ts) {
	Message msg;
	msg.type = MessageType::MSG_COMMIT_ACK;
	msg.source = source;
	msg.destination = destination;

	msg.msg_id = Generator::get_next_msg_id();

	MsgCommitAckPayload payload{
			.txid=txid,
			.commit_ts=commit_ts
	};

	msg.payload.payload = payload;

	return msg;
}
Message CreateMsgRolledBackByServer(const ActorID source, const ActorID destination, const TxID txid, const TxID conflict_txid) {
	Message msg;
	msg.type = MessageType::MSG_ROLLED_BACK_BY_SERVER;
	msg.source = source;
	msg.destination = destination;

	msg.msg_id = Generator::get_next_msg_id();

	MsgRolledBackByServerPayload payload{
			.txid=txid,
			.conflict_txid=conflict_txid
	};

	msg.payload.payload = payload;

	return msg;
}

} // namespace sdb::tx::msg