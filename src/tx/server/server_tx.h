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

std::string to_string(const ServerTXState state);

class ServerTX final {
public:
	ServerTX(ActorID clientID, Storage* storage);

	void tick(Timestamp tx, Messages msg, Messages* outgoing_msgs);

private:
	const ActorID clientID_;
	Storage* storage_;

	ServerTXState state_;
};


} // namespace sdb::tx
