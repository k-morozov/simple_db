//
// Created by focus on 2/10/24.
//

#include <gtest/gtest.h>

#include <tx/runtime.h>
#include <tx/actor.h>
#include <tx/msg/message.h>

class FakeActor: public sdb::tx::IActor {
public:
	sdb::tx::ActorID get_actor_id() const override {
		return 1;
	}
	void send_on_tick(sdb::tx::Messages&& _) override {

	}
};

TEST(TestActor, RegisterTwiceFailed) {
	sdb::tx::Runtime runtime;

	auto actor1 = FakeActor();
	auto actor2 = FakeActor();

	ASSERT_NO_THROW(runtime.register_actor(&actor1));
	ASSERT_ANY_THROW(runtime.register_actor(&actor2););
}