//
// Created by focus on 2/12/24.
//

#include "server.h"

#include <tx/clock/clock.h>

namespace sdb::tx {

namespace {

TxID get_txid_from_msg_payload(const Message& msg) {
	switch (msg.type) {
		case MessageType::MSG_UNDEFINED:
			return UNDEFINED_TX_ID;
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

void Server::send_on_tick(Clock& clock, Messages &&income_msgs) {
	std::unordered_map<TxID, Messages> messages_per_tx;

	for (const auto& msg : income_msgs) {
		const auto txid = get_txid_from_msg_payload(msg);
		if (txid == UNDEFINED_TX_ID) {
			throw std::invalid_argument("msg with undefined txid.");
		}
		messages_per_tx[txid].push_back(msg);
	}

	Messages outgoing_messages;
	for(auto& [txid, tx_msgs] : messages_per_tx) {
		auto* server_tx = get_or_create_tx(txid);
		server_tx->tick(clock.next(), std::move(tx_msgs), &outgoing_messages);
	}

	for(auto& msg : outgoing_messages) {
		runtime_.send(std::move(msg));
	}
}

ServerTX* Server::get_or_create_tx(const TxID txid) {
	if (!transactions_.contains(txid)) {
		transactions_.emplace(txid, ServerTX(get_actor_id(), &storage_));
	}

	return &transactions_.at(txid);
}

} // namespace sdb::tx