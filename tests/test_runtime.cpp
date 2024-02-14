//
// Created by focus on 2/10/24.
//

#include <gtest/gtest.h>

#include <tx/runtime/runtime.h>
#include <tx/runtime/proxy_runtime.h>
#include <tx/actor.h>
#include <tx/msg/message.h>
#include <tx/msg/sent_message.h>

sdb::tx::Messages send_to_runtime(sdb::tx::RuntimePtr runtime,
								  const int count_msgs_from_actor,
								  const sdb::tx::ActorID source,
								  const sdb::tx::ActorID destination,
								  const size_t msg_id_offset) {
	auto proxy = sdb::tx::ProxyRuntime(std::move(runtime), source);
	sdb::tx::Messages expected_msg;

	for(int i=0; i<count_msgs_from_actor; i++) {
		sdb::tx::Message msg;
		msg.type = sdb::tx::MessageType::MSG_START;
		msg.source = source;
		msg.destination = destination;
		msg.id = msg_id_offset + i;

		proxy.send(msg);
		expected_msg.push_back(msg);
	}
	return expected_msg;
}

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

TEST(TestActorRuntime, CmpMsg) {
	sdb::tx::Message msg1;
	msg1.type = sdb::tx::MessageType::MSG_START;
	msg1.source = 1;
	msg1.destination = 1;
	msg1.id = 1;

	ASSERT_EQ(msg1, msg1);

	sdb::tx::Message msg2;
	msg2.type = sdb::tx::MessageType::MSG_START;
	msg2.source = 1;
	msg2.destination = 1;
	msg2.id = 1;

	ASSERT_EQ(msg1, msg2);
	ASSERT_EQ(msg2, msg1);

	{
		auto msg_another = msg2;
		msg_another.destination++;
		ASSERT_FALSE(msg2 == msg_another);
	}

	{
		auto msg_another = msg2;
		msg_another.source++;
		ASSERT_FALSE(msg2 == msg_another);
	}

	{
		auto msg_another = msg2;
		msg_another.id++;
		ASSERT_FALSE(msg2 == msg_another);
	}

	{
		auto msg_another = msg2;
		msg_another.type = sdb::tx::MessageType::MSG_UNDEFINED;
		ASSERT_FALSE(msg2 == msg_another);
	}
}

TEST(TestActorRuntime, RegisterTwiceFailed) {
	sdb::tx::Runtime runtime;

	GeneratorActorID builder(GeneratorActorID::Type::EQUAL_ACTOR_ID);

	auto actor1 = FakeActor(builder);
	auto actor2 = FakeActor(builder);

	ASSERT_NO_THROW(runtime.register_actor(&actor1));
	ASSERT_ANY_THROW(runtime.register_actor(&actor2););
}

TEST(TestActorRuntime, SendMsgToOneActor) {
	sdb::tx::Runtime runtime;

	GeneratorActorID builder;

	auto actor1 = FakeActor(builder);
	auto actor2 = FakeActor(builder);

	ASSERT_NO_THROW(runtime.register_actor(&actor1));
	ASSERT_NO_THROW(runtime.register_actor(&actor2));

	constexpr int count_msgs_actor2 = 3;
	sdb::tx::Messages expected_actor2;
	expected_actor2.reserve(count_msgs_actor2);

	for(int i=0; i<count_msgs_actor2; i++) {
		sdb::tx::Message msg;
		msg.type = sdb::tx::MessageType::MSG_START;
		msg.source = actor1.get_actor_id();
		msg.destination = actor2.get_actor_id();
		msg.id = i;

		runtime.send(msg);
		expected_actor2.push_back(msg);
	}
	std::sort(expected_actor2.begin(), expected_actor2.end());

	runtime.run(25);

	ASSERT_TRUE(actor1.total.empty());
	ASSERT_FALSE(actor2.total.empty());

	ASSERT_TRUE(actor2.total.size() == count_msgs_actor2);

	auto result = actor2.total;
	std::sort(result.begin(), result.end());
	ASSERT_TRUE(result == expected_actor2);
}

TEST(TestActorRuntime, SendMsgTwoActors) {
	auto runtime = std::make_shared<sdb::tx::Runtime>();

	GeneratorActorID builder;

	auto actor1 = FakeActor(builder);
	auto actor2 = FakeActor(builder);

	ASSERT_NO_THROW(runtime->register_actor(&actor1));
	ASSERT_NO_THROW(runtime->register_actor(&actor2));

	constexpr int count_msgs_actor2 = 10;
	sdb::tx::Messages expected_msg_to_actor2;
	expected_msg_to_actor2.reserve(count_msgs_actor2);

	{
		auto expected_msg = send_to_runtime(runtime,
											count_msgs_actor2,
											actor1.get_actor_id(),
											actor2.get_actor_id(),
											0);
		std::move(expected_msg.begin(), expected_msg.end(),
				  std::back_inserter(expected_msg_to_actor2));
	}

	constexpr int count_msgs_actor1 = 15;
	sdb::tx::Messages expected_msg_to_actor1;
	expected_msg_to_actor1.reserve(count_msgs_actor1);
	{
		auto expected_msg = send_to_runtime(runtime,
											count_msgs_actor1,
											actor2.get_actor_id(),
											actor1.get_actor_id(),
											count_msgs_actor2);
		std::move(expected_msg.begin(), expected_msg.end(),
				  std::back_inserter(expected_msg_to_actor1));
	}

	std::sort(expected_msg_to_actor2.begin(), expected_msg_to_actor2.end());
	std::sort(expected_msg_to_actor1.begin(), expected_msg_to_actor1.end());

	runtime->run(25);

	{
		auto result1 = actor1.total;
		std::sort(result1.begin(), result1.end());
		ASSERT_TRUE(actor1.total.size() == count_msgs_actor1);
		ASSERT_TRUE(result1 == expected_msg_to_actor1);
	}

	{
		auto result2 = actor2.total;
		std::sort(result2.begin(), result2.end());
		ASSERT_TRUE(result2.size() == count_msgs_actor2);
		ASSERT_TRUE(result2 == expected_msg_to_actor2);
	}
}

TEST(TestActorRuntime, SendSomeMsgsToOneActor) {
	auto runtime = std::make_shared<sdb::tx::Runtime>();

	GeneratorActorID builder;

	auto actor1 = FakeActor(builder);
	auto actor2 = FakeActor(builder);
	auto actor3 = FakeActor(builder);

	ASSERT_NO_THROW(runtime->register_actor(&actor1));
	ASSERT_NO_THROW(runtime->register_actor(&actor2));
	ASSERT_NO_THROW(runtime->register_actor(&actor3));

	sdb::tx::Messages expected_msg;

	constexpr int count_msgs_from_actor1 = 10;
	{
		auto expected_msg_to_actor3 = send_to_runtime(runtime,
											 count_msgs_from_actor1,
											 actor1.get_actor_id(),
											 actor3.get_actor_id(),
											 0);
		std::move(expected_msg_to_actor3.begin(), expected_msg_to_actor3.end(),
				  std::back_inserter(expected_msg));
	}

	constexpr int count_msgs_from_actor2 = 15;
	{
		auto expected_msg_to_actor3 = send_to_runtime(runtime,
													  count_msgs_from_actor2,
													  actor2.get_actor_id(),
													  actor3.get_actor_id(),
													  count_msgs_from_actor1);
		std::move(expected_msg_to_actor3.begin(), expected_msg_to_actor3.end(),
				  std::back_inserter(expected_msg));
	}

	std::sort(expected_msg.begin(), expected_msg.end());
	runtime->run(25);

	ASSERT_TRUE(actor1.total.empty());
	ASSERT_TRUE(actor2.total.empty());

	ASSERT_TRUE(actor3.total.size() == count_msgs_from_actor1 + count_msgs_from_actor2);

	auto result = actor3.total;
	std::sort(result.begin(), result.end());
	ASSERT_TRUE(result == expected_msg);
}