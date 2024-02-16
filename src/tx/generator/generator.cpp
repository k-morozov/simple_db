//
// Created by focus on 2/15/24.
//

#include "generator.h"

namespace sdb::tx {

static TxID txid = 1'000;

MsgID Generator::get_next_msg_id() {
	static MsgID msgID = 0;
	return ++msgID;
}

TxID Generator::get_next_tx_id() {
	return ++txid;
}

TxID Generator::get_current_tx_id() {
	return txid;
}

} // namespace sdb::tx