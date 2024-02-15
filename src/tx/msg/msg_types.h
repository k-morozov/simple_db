//
// Created by focus on 2/13/24.
//

#pragma once

#include <ostream>

#include <tx/types.h>

namespace sdb::tx::msg {

struct MsgStartPayload {
	TxID txid;
	Timestamp ts;
};

bool operator==(const MsgStartPayload& lhs, const MsgStartPayload& rhs);
std::ostream& operator<<(std::ostream& stream, const MsgStartPayload& payload);

struct MsgAckStartPayload {
	TxID txid;
	Timestamp read_ts;
};

bool operator==(const MsgAckStartPayload& lhs, const MsgAckStartPayload& rhs);
std::ostream& operator<<(std::ostream& stream, const MsgAckStartPayload& payload);

} // namespace sdb::tx::msg