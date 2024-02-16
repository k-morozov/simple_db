//
// Created by focus on 2/15/24.
//

#pragma once

#include <tx/types.h>

namespace sdb::tx {

class Generator final {
public:
	static MsgID get_next_msg_id();
	static TxID get_next_tx_id();
};

} // namespace sdb::tx