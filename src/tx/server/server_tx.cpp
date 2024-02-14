//
// Created by focus on 2/14/24.
//

#include "server_tx.h"

#include <cassert>

#include <common/log/log.h>
#include <tx/msg/message.h>
#include <tx/clock/clock.h>

namespace sdb::tx {

std::string to_string(const ServerTXState state) {
	switch (state) {
		case ServerTXState::NOT_STARTED:
			return "NOT_STARTED";
		case ServerTXState::OPEN:
			return "OPEN";
	}
}

ServerTX::ServerTX(ActorID clientID, Storage *storage):
	clientID_(clientID),
	storage_(storage)
{
	LOG_DEBUG << "ServerTX with clientID=" << clientID << " created";
}

void ServerTX::tick(const Timestamp tx, Messages msgs, Messages* outgoing_msgs) {
	assert(outgoing_msgs);

	for(auto& msg : msgs) {
		LOG_DEBUG << "[ServerTX::tick] state=" << to_string(state_)
				  << " got msg=" << msg.type << " with msg_id=" << msg.id;
	}
}


} // namespace sdb::tx