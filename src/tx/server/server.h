//
// Created by focus on 2/12/24.
//

#pragma once

#include <unordered_map>

#include <ostream>
#include <tx/actor.h>
#include <tx/runtime/proxy_runtime.h>
#include <tx/retrier/retrier.h>
#include <tx/server/server_tx.h>
#include <tx/storage/storage.h>

namespace sdb::tx::server {

class Server: public IActor {
public:
	using Transactions = std::unordered_map<TxID, ServerTX>;

	Server(ActorID actor_id, const KeyIntervals& key_intervals, ProxyRuntime runtime);
	~Server() override = default;

	ActorID get_actor_id() const override;

	void on_tick(Clock& clock, Messages&& msgs) override;
private:
	const ActorID actor_id_;
	const KeyIntervals key_intervals_;
	ProxyRuntime proxy_runtime_;

	// @TODO storage
	Storage storage_;
	Retrier retrier_;

	Transactions transactions_;

	ServerTX* get_or_create_tx(const TxID txid);
};

} // namespace sdb::tx::server

