//
// Created by focus on 2/15/24.
//

#include "client.h"

#include <common/log/log.h>
#include <tx/client/client_tx.h>
#include <tx/client/tx_spec.h>
#include <tx/types.h>

#include <cassert>
#include <unordered_map>

namespace sdb::tx::client {

std::ostream& operator<<(std::ostream& stream, const Client& client) {
  stream << "[Client]"
         << "[actor_id=" << client.get_actor_id() << "]";

  return stream;
}

Client::Client(ActorID actor_id, const std::vector<TxSpec>& tx_specs,
               const Discovery* discovery, ProxyRuntime proxy)
    : actor_id_(actor_id),
      discovery_(discovery),
      proxy_(std::move(proxy)),
      retrier_() {
  transactions_.reserve(tx_specs.size());
  for (const auto& tx_spec : tx_specs) {
    transactions_.emplace_back(
        std::make_unique<ClientTx>(actor_id_, tx_spec, discovery, &retrier_));
  }
  LOG_SELF_DEBUG << "created.";
}

const std::unique_ptr<ClientTx>& Client::get_tx(const size_t index) const {
  return transactions_[index];
}

ActorID Client::get_actor_id() const { return actor_id_; }

void Client::on_tick(Clock& clock, Messages&& msgs) {
  std::unordered_map<TxID, Messages> messages_per_tx;

  for (const auto& msg : msgs) {
    const auto txid = msg::get_txid_from_msg_payload(msg);
    LOG_DEBUG << "[Client::on_tick] msg_id=" << msg.msg_id
              << " has txid=" << txid;

    if (txid != UNDEFINED_TX_ID) {
      messages_per_tx[txid].push_back(msg);
    }
  }

  Messages scheduled_msgs;

  for (const auto& tx : transactions_) {
    const auto txid = tx->get_tx_id();
    LOG_DEBUG << "[Client::on_tick][txid=" << txid << "] process.";
    Messages tx_msgs;

    if (txid != UNDEFINED_TX_ID) {
      auto it = messages_per_tx.find(txid);
      if (it != messages_per_tx.end()) {
        tx_msgs = std::move(it->second);
      }
    }

    tx->tick(clock.next(), tx_msgs, &scheduled_msgs);
  }

  //	for(const auto& [txid, tx_msgs] : messages_per_tx) {
  //		assert(tx_msgs.empty());
  //	}

  for (auto& msg : scheduled_msgs) {
    proxy_.send(msg);
  }
}

}  // namespace sdb::tx::client