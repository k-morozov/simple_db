//
// Created by focus on 2/12/24.
//

#include "server.h"

namespace sdb::tx {

namespace {

TxID get_txid_from_msg_payload(const Message& msg) {
	switch (msg.type) {
		case MessageType::MSG_UNDEFINED:
			throw std::invalid_argument("broken payload in msg to get txid.");
		case MessageType::MSG_START:
			return msg.payload.get<StartPayload>().txid;
	}
}

}

Server::Server(ActorID actor_id,
			   const std::vector<KeyInterval>& key_intervals,
			   ProxyRuntime runtime) :
		actor_id_(actor_id),
		key_intervals_(key_intervals),
		runtime_(std::move(runtime))
{}

ActorID Server::get_actor_id() const {
	return actor_id_;
}

void Server::send_on_tick(Messages &&msgs) {
	std::unordered_map<TxID, Messages> messages_per_tx;
}

} // namespace sdb::tx