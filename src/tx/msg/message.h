//
// Created by focus on 2/10/24.
//

#pragma once

#include <cstdint>
#include <string>

#include <tx/types.h>
#include <tx/msg/msg_types.h>
#include <tx/msg/msg_payload.h>

namespace sdb::tx::msg {

enum class MessageType: uint8_t {
	MSG_UNDEFINED,
	MSG_START,
	MSG_START_ACK,
};

std::ostream& operator<<(std::ostream& stream, MessageType type);

// @todo change variant
struct Message {
	MessageType type{MessageType::MSG_UNDEFINED};
	ActorID source;
	ActorID destination;

	MsgID msg_id;

	MsgPayload payload;
};

std::ostream& operator<<(std::ostream& stream, const Message& msg);
bool operator==(const Message& lhs, const Message& rhs);
bool operator<(const Message& lhs, const Message& rhs);

TxID get_txid_from_msg_payload(const msg::Message& msg);

Message CreateMsgStart(ActorID source, ActorID destination);
// @todo create from msg
Message CreateMsgStartAck(ActorID source, ActorID destination, TxID txid, Timestamp read_ts);

} // namespace sdb::tx::msg