//
// Created by focus on 2/10/24.
//

#pragma once

#include <concepts>
#include <expected>
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
	MSG_PUT,
	MSG_PUT_REPLY,
	MSG_COMMIT,
	MSG_COMMIT_ACK,
	MSG_ROLLED_BACK_BY_SERVER
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

/*
 * CreateMsgStart generates a txid.
 */
Message CreateMsgStart(ActorID source, ActorID destination);
// @todo create from msg
Message CreateMsgStartAck(ActorID source, ActorID destination, TxID txid, Timestamp read_ts);

Message CreateMsgPut(ActorID source, ActorID destination, TxID txid, Key key, Value value);
Message CreateMsgPutReply(ActorID source, ActorID destination, TxID txid, MsgID msg_id);

Message CreateMsgCommit(ActorID source, ActorID destination, TxID txid);
Message CreateMsgCommitAck(ActorID source, ActorID destination, TxID txid, Timestamp commit_ts);
Message CreateMsgRolledBackByServer(ActorID source, ActorID destination, TxID txid, TxID conflict_txid);

} // namespace sdb::tx::msg