//
// Created by focus on 2/12/24.
//

#include "server.h"

#include <common/log/log.h>
#include <tx/clock/clock.h>

namespace sdb::tx::server {

Server::Server(ActorID actor_id,
			   const KeyIntervals& key_intervals,
			   ProxyRuntime proxy) :
		actor_id_(actor_id),
		key_intervals_(key_intervals),
		proxy_runtime_(std::move(proxy)),
		retrier_()
{}

ActorID Server::get_actor_id() const {
	return actor_id_;
}

void Server::on_tick(Clock& clock, Messages &&income_msgs) {
	LOG_DEBUG << "[Server::on_tick] call.";
	std::unordered_map<TxID, Messages> messages_per_tx;

	for (const auto& msg : income_msgs) {
		const auto txid = msg::get_txid_from_msg_payload(msg);
		if (txid == UNDEFINED_TX_ID) {
			throw std::invalid_argument("msg with undefined txid.");
		}
		messages_per_tx[txid].push_back(msg);
	}

	Messages scheduled_messages;
	for(auto& [txid, tx_msgs] : messages_per_tx) {
		auto* server_tx = get_or_create_tx(txid);
		server_tx->tick(clock.next(), std::move(tx_msgs), &scheduled_messages);
	}

	for(auto& msg : scheduled_messages) {
		proxy_runtime_.send(msg);
	}
}

ServerTX* Server::get_or_create_tx(const TxID txid) {
	if (!transactions_.contains(txid)) {
		transactions_.emplace(txid, ServerTX(get_actor_id(), &storage_, &retrier_));
	}

	return &transactions_.at(txid);
}

} // namespace sdb::tx::server