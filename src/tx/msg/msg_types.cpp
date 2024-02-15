//
// Created by focus on 2/15/24.
//

#include "msg_types.h"

#include <tuple>

namespace sdb::tx::msg {

bool operator==(const MsgStartPayload& lhs, const MsgStartPayload& rhs) {
	return std::make_tuple(lhs.txid, lhs.ts)
	==
	std::make_tuple(rhs.txid, rhs.ts);
}

std::ostream& operator<<(std::ostream& stream, const MsgStartPayload& payload) {
	stream << "MsgStartPayload: txid=" << payload.txid
		<< ", ts=" << payload.ts;
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

} // namespace sdb::tx::msg