//
// Created by focus on 2/15/24.
//

#pragma once

#include <tx/actor.h>
#include <tx/msg/message.h>

namespace common {

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

} // namespace common