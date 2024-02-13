//
// Created by focus on 2/13/24.
//

#pragma once

#include <tx/types.h>

namespace sdb::tx {

struct StartPayload {
	TxID txid;
};

struct AckStartPayload {
	TxID txid;
};

} // namespace sdb::tx