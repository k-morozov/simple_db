//
// Created by focus on 2/10/24.
//

#include <gtest/gtest.h>

#include <tx/actor/runtime.h>
#include <tx/actor/actor.h>
#include <tx/actor/msg/message.h>

class FakeActor: public sdb::actor::IActor {
public:
	sdb::actor::ActorID get_actor_id() const override {
		return 1;
	}
	void send_on_tick(sdb::actor::Messages&& _) override {

	}
};

TEST(TestActor, RegisterTwiceFailed) {
	sdb::actor::Runtime runtime;

	auto actor1 = FakeActor();
	auto actor2 = FakeActor();

	ASSERT_NO_THROW(runtime.register_actor(&actor1));
	ASSERT_ANY_THROW(runtime.register_actor(&actor2););
}