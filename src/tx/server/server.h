//
// Created by focus on 2/12/24.
//

#pragma once

#include <unordered_map>

#include <tx/actor.h>
#include <tx/runtime/proxy_runtime.h>
#include <tx/server/retrier.h>
#include <tx/server/server_tx.h>
#include <tx/storage/storage.h>

namespace sdb::tx {

// [start, end)
struct KeyInterval final {
	Key start;
	Key end;
};

class Server final: public IActor {
public:
	using Transactions = std::unordered_map<TxID, ServerTX>;

	~Server() override = default;
	Server(ActorID actor_id, const std::vector<KeyInterval>& key_intervals, ProxyRuntime runtime);

	ActorID get_actor_id() const override;

	void send_on_tick(Clock& clock, Messages&& msgs) override;
private:
	const ActorID actor_id_;
	const std::vector<KeyInterval> key_intervals_;
	ProxyRuntime proxy_runtime_;

	// @TODO storage
	Storage storage_;
	Retrier retrier_;

	Transactions transactions_;

	ServerTX* get_or_create_tx(const TxID txid);
};

} // namespace sdb::tx

