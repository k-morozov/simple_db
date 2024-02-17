//
// Created by focus on 2/15/24.
//

#include "client.h"

#include <cassert>
#include <unordered_map>

#include <common/log/log.h>
#include <tx/types.h>
#include <tx/client/tx_spec.h>
#include <tx/client/client_tx.h>

namespace sdb::tx::client {

std::ostream& operator<<(std::ostream& stream, const Client& client) {
	stream << "[Client]"
		<< "[actor_id=" << client.get_actor_id() << "]";
	return stream;
}

Client::Client(ActorID actor_id, const std::vector<TxSpec>& tx_specs, const Discovery* discovery, ProxyRuntime proxy):
	actor_id_(actor_id),
	discovery_(discovery),
	proxy_(std::move(proxy)),
	retrier_()
{
	transactions_.reserve(tx_specs.size());
	for (const auto& tx_spec : tx_specs) {
		transactions_.emplace_back(std::make_unique<ClientTx>(actor_id_, tx_spec, discovery, &retrier_));
	}
	LOG_SELF_DEBUG  << "created.";
}

ActorID Client::get_actor_id() const {
	return actor_id_;
}

void Client::send_on_tick(Clock& clock, Messages&& msgs) {
	std::unordered_map<TxID, Messages> messages_per_tx;

	for(const auto& msg : msgs) {
		const auto txid = msg::get_txid_from_msg_payload(msg);
		LOG_SELF_DEBUG << "msg_id=" << msg.msg_id
			<< " has txid=" << txid;

		if (txid != UNDEFINED_TX_ID) {
			messages_per_tx[txid].push_back(msg);
		}
	}

	Messages outgoing_msgs;

	for(const auto& tx : transactions_) {
		const auto txid = tx->get_tx_id();
		Messages  tx_msgs;

		if (txid != UNDEFINED_TX_ID) {
			auto it = messages_per_tx.find(txid);
			if (it != messages_per_tx.end()) {
				tx_msgs = std::move(it->second);
			}
		}

		tx->tick(clock.next(), tx_msgs, &outgoing_msgs);
	}

//	for(const auto& [txid, tx_msgs] : messages_per_tx) {
//		assert(tx_msgs.empty());
//	}

	for(auto& msg : outgoing_msgs) {
		proxy_.send(msg);
	}
}

} // namespace sdb::tx::client