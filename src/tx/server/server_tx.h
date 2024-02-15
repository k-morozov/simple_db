//
// Created by focus on 2/14/24.
//

#pragma once

#include <string>

#include <tx/types.h>

namespace sdb::tx {

class Storage;

enum class ServerTXState {
	NOT_STARTED,
	OPEN
};

std::string to_string(ServerTXState state);

class ServerTX final {
public:
	ServerTX(ActorID clientID, Storage* storage);

	void tick(Timestamp ts, Messages msg, Messages* outgoing_msgs);

private:
	const ActorID clientID_;
	Storage* storage_;

	ServerTXState state_;

	TxID txid_{UNDEFINED_TX_ID};
	Timestamp read_ts_;
	Timestamp commit_ts_;

	void report_unexpected_msg(msg::Message msg);
	void process_msg_not_started(Timestamp ts, msg::Message msg);
};


} // namespace sdb::tx
