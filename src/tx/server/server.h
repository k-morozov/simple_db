//
// Created by focus on 2/12/24.
//

#pragma once

#include <unordered_map>

#include <tx/actor.h>
#include <tx/runtime/proxy_runtime.h>

namespace sdb::tx {

// [start, end)
struct KeyInterval final {
	Key start;
	Key end;
};

class Server final: public IActor {
public:
	~Server() override = default;
	Server(ActorID actor_id, const std::vector<KeyInterval>& key_intervals, ProxyRuntime runtime);

	ActorID get_actor_id() const override;

	void send_on_tick(Messages &&msgs) override;
private:
	const ActorID actor_id_;
	const std::vector<KeyInterval> key_intervals_;
	ProxyRuntime runtime_;

	// @TODO storage
	std::unordered_map<TxID, int> transactions_;
};

} // namespace sdb::tx

