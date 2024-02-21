//
// Created by focus on 2/14/24.
//

#pragma once

#include <string>

#include <tx/types.h>

namespace sdb::tx {
	class Storage;
	class Retrier;
}

namespace sdb::tx::server {

enum class ServerTXState {
	NOT_STARTED,
	OPEN,
	COMMITTED,
	ROLLED_BACK_BY_SERVER
};

std::string to_string(ServerTXState state);

class ServerTX final {
public:
	ServerTX(ActorID clientID, Storage* storage, Retrier* retrier);

	void tick(Timestamp ts, Messages msg, Messages* outgoing_msgs);

private:
	const ActorID clientID_;
	Storage* storage_;
	Retrier* retrier_;

	ServerTXState state_{ServerTXState::NOT_STARTED};

	TxID txid_{UNDEFINED_TX_ID};
	Timestamp read_ts_{UNDEFINED_TS};
	Timestamp commit_ts_{UNDEFINED_TS};

	void report_unexpected_msg(msg::Message msg);
	void process_msg_not_started(Timestamp ts, msg::Message msg);
	void process_msg_open(Timestamp ts, msg::Message msg);

	struct DataPut {
		Key key;
		Value value;
	};
	std::vector<DataPut> puts_;
};


} // namespace sdb::tx::server
