//
// Created by focus on 2/15/24.
//

#pragma once

#include <vector>
#include <ostream>

#include <tx/types.h>

namespace sdb::tx::client {

/*
 *
 * ClientTxSpec describes what should be done in a given
 * transaction. It lists all Get and Put requests that the client
 * will perform in the transaction.
 * It also specifies whether the transaction is committed or rolled
 * back in the end.
 *
 * */

struct ClientTxGet {
	Timestamp earliest_ts;
	Key key;
};

struct ClientTxPut {
	Timestamp earliest_ts;
	Key key;
	Value value;
};

struct ClientTxSpec final {
	// Earliest timestamp to start the transaction.
	Timestamp earliest_start_ts;

	// Earliest timestamp to commit the transaction.
	Timestamp earliest_commit_ts;

	std::vector<ClientTxGet> gets;
	std::vector<ClientTxPut> puts;

	enum class State {
		UNDEFINED,
		COMMIT,
		ROLLBACK,
	};

	State action{State::UNDEFINED};
};

std::ostream& operator<<(std::ostream& stream, const ClientTxSpec& spec);

} // namespace sdb::tx::client
