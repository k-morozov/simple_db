//
// Created by focus on 2/13/24.
//

#pragma once

#include <ostream>

#include <tx/types.h>

namespace sdb::tx::msg {

struct MsgStartPayload {
	TxID txid;
	Timestamp read_ts;
};

bool operator==(const MsgStartPayload& lhs, const MsgStartPayload& rhs);
std::ostream& operator<<(std::ostream& stream, const MsgStartPayload& payload);

struct MsgAckStartPayload {
	TxID txid;
	Timestamp read_ts;
};

bool operator==(const MsgAckStartPayload& lhs, const MsgAckStartPayload& rhs);
std::ostream& operator<<(std::ostream& stream, const MsgAckStartPayload& payload);

struct MsgPutPayload {
	TxID txid;
	Key key;
	Value value;
};

bool operator==(const MsgPutPayload& lhs, const MsgPutPayload& rhs);
std::ostream& operator<<(std::ostream& stream, const MsgPutPayload& payload);

struct MsgPutReplyPayload {
	TxID txid;
	// original_msg_id
	MsgID msg_id;
};

bool operator==(const MsgPutReplyPayload& lhs, const MsgPutReplyPayload& rhs);
std::ostream& operator<<(std::ostream& stream, const MsgPutReplyPayload& payload);

struct MsgCommitPayload {
	TxID txid;
	// Participants except the coordinator.
};

bool operator==(const MsgCommitPayload& lhs, const MsgCommitPayload& rhs);
std::ostream& operator<<(std::ostream& stream, const MsgCommitPayload& payload);

} // namespace sdb::tx::msg