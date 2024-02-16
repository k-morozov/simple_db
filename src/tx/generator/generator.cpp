//
// Created by focus on 2/15/24.
//

#include "generator.h"

namespace sdb::tx {

MsgID Generator::get_next_msg_id() {
	static MsgID msgID = 0;
	return ++msgID;
}

TxID Generator::get_next_tx_id() {
	static TxID txid = 1'000;
	return ++txid;
}

} // namespace sdb::tx