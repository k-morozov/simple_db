//
// Created by focus on 2/15/24.
//

#pragma once

#include <tx/client/client.h>
#include <tx/server/server.h>
#include <tx/actor.h>
#include <tx/msg/message.h>

namespace common {

using namespace sdb::tx;

class GeneratorActorID final {
public:
	enum class Type {
		EQUAL_ACTOR_ID,
		DIFFERENT_ACTOR_ID
	};
	explicit GeneratorActorID(Type type = Type::DIFFERENT_ACTOR_ID) : type(type) {};
	sdb::tx::ActorID operator()() {
		const auto result = actor_id;
		if (type == Type::DIFFERENT_ACTOR_ID) {
			actor_id++;
		}
		return result;
	}
private:
	const Type type;
	sdb::tx::ActorID actor_id{1};
};

class FakeActor: public sdb::tx::IActor {
public:
	explicit FakeActor(GeneratorActorID& build) : actor_id(build()) {};

	sdb::tx::ActorID get_actor_id() const override {
		return actor_id;
	}
	void send_on_tick(sdb::tx::Clock& clock, sdb::tx::Messages&& msgs) override {
		std::move(std::begin(msgs), std::end(msgs), std::back_inserter(total));
	}

	sdb::tx::Messages total;

private:
	const sdb::tx::ActorID actor_id;
};

class TestClient: public client::Client {
public:
	explicit TestClient(ActorID actor_id,
						const std::vector<client::TxSpec>& tx_specs,
						const Discovery* discovery,
						ProxyRuntime proxy) :
			Client(actor_id, tx_specs, discovery, proxy)
	{};

	void send_on_tick(sdb::tx::Clock& clock, sdb::tx::Messages&& msgs) override {
		total.clear();
		std::copy(std::begin(msgs), std::end(msgs), std::back_inserter(total));
		client::Client::send_on_tick(clock, std::move(msgs));
	}

	sdb::tx::Messages total;
};

class TestServer: public Server {
public:
	TestServer(ActorID actor_id, const KeyIntervals& key_intervals, ProxyRuntime runtime)
		: Server(actor_id, key_intervals, runtime) {};

	void send_on_tick(sdb::tx::Clock& clock, sdb::tx::Messages&& msgs) override {
		std::copy(std::begin(msgs), std::end(msgs), std::back_inserter(total));
		Server::send_on_tick(clock, std::move(msgs));
	}
	sdb::tx::Messages total;
};

} // namespace common