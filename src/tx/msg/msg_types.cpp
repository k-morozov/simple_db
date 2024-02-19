//
// Created by focus on 2/15/24.
//

#include "msg_types.h"

#include <tuple>

namespace sdb::tx::msg {

bool operator==(const MsgStartPayload& lhs, const MsgStartPayload& rhs) {
	return std::make_tuple(lhs.txid, lhs.read_ts)
	==
	std::make_tuple(rhs.txid, rhs.read_ts);
}

std::ostream& operator<<(std::ostream& stream, const MsgStartPayload& payload) {
	stream << "MsgStartPayload: txid=" << payload.txid
		<< ", read_ts=" << payload.read_ts;
	return stream;
}

bool operator==(const MsgAckStartPayload& lhs, const MsgAckStartPayload& rhs) {
	return std::make_tuple(lhs.txid, lhs.read_ts)
	==
	std::make_tuple(rhs.txid, rhs.read_ts);
}

std::ostream& operator<<(std::ostream& stream, const MsgAckStartPayload& payload) {
	stream << "MsgAckStartPayload: txid=" << payload.txid
		   << ", read_ts=" << payload.read_ts;
	return stream;
}

bool operator==(const MsgPutPayload& lhs, const MsgPutPayload& rhs) {
	return std::make_tuple(lhs.txid, lhs.key, lhs.value)
		   ==
		   std::make_tuple(rhs.txid, rhs.key, rhs.value);
}
std::ostream& operator<<(std::ostream& stream, const MsgPutPayload& payload) {
	stream << "MsgPutPayload: txid=" << payload.txid
		   << ", key=" << payload.key
		   << ", value=" << payload.value;
	return stream;
}

bool operator==(const MsgPutReplyPayload& lhs, const MsgPutReplyPayload& rhs) {
	return std::make_tuple(lhs.txid, lhs.msg_id)
		   ==
		   std::make_tuple(rhs.txid, rhs.msg_id);
}
std::ostream& operator<<(std::ostream& stream, const MsgPutReplyPayload& payload) {
	stream << "MsgPutReplyPayload: txid=" << payload.txid
		   << ", msg_id=" << payload.msg_id;
	return stream;
}

bool operator==(const MsgCommitPayload& lhs, const MsgCommitPayload& rhs) {
	return std::make_tuple(lhs.txid)
		   ==
		   std::make_tuple(rhs.txid);
}

std::ostream& operator<<(std::ostream& stream, const MsgCommitPayload& payload) {
	stream << "MsgCommitPayload: txid=" << payload.txid;
	return stream;
}

} // namespace sdb::tx::msg