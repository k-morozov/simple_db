//
// Created by focus on 2/15/24.
//

#pragma once

#include <memory>

#include <tx/actor.h>
#include <tx/runtime/proxy_runtime.h>
#include <tx/retrier/retrier.h>
#include <tx/discovery/discovery.h>
#include <tx/client/client_tx.h>

namespace sdb::tx::client {

class TxSpec;

class Client: public IActor {
public:
	Client(ActorID actor_id,
		   const std::vector<TxSpec>& tx_specs,
		   const Discovery* discovery,
		   ProxyRuntime proxy);
	~Client() override = default;

	ActorID get_actor_id() const override;

	void send_on_tick(Clock &clock, Messages &&msgs) override;

private:
	const ActorID actor_id_;
	const Discovery* discovery_;
	ProxyRuntime proxy_;
	Retrier retrier_;

	std::vector<std::unique_ptr<ClientTx>> transactions_;
};


} // namespace sdb::tx::client
