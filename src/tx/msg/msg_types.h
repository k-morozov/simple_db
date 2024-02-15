//
// Created by focus on 2/13/24.
//

#pragma once

#include <tx/types.h>

namespace sdb::tx::msg {

struct MsgStartPayload {
	TxID txid;
	Timestamp ts;
};

struct MsgAckStartPayload {
	TxID txid;
	Timestamp read_ts;
};

} // namespace sdb::tx::msg