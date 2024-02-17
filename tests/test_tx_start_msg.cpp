//
// Created by focus on 2/15/24.
//

#include <gtest/gtest.h>

#include <memory>

#include <tx/runtime/runtime.h>
#include <tx/runtime/proxy_runtime.h>
#include <tx/server/server.h>
#include <tx/clock/clock.h>
#include <tx/generator/generator.h>
#include <tx/discovery/discovery.h>
#include <tx/client/client.h>
#include <tx/client/tx_spec.h>

#include "common/fake_actor.h"

using namespace sdb::tx;
using Message = msg::Message;

bool LightCmpMsg(const Message& lhs, const Message& rhs) {
	if (lhs.type != rhs.type)
		return false;
	if (lhs.source != rhs.source)
		return false;
	if (lhs.destination != rhs.destination)
		return false;
	if (lhs.msg_id != rhs.msg_id)
		return false;
	if (lhs.payload.payload.index() != rhs.payload.payload.index())
		return false;

	return true;
}

class RuntimeTxStartMsgFixture : public ::testing::Test {
public:
	void SetUp() override {
		runtime = std::make_shared<Runtime>();
		server_actor_id = builder();
		proxy_server = std::make_unique<ProxyRuntime>(runtime, server_actor_id);
		server = std::make_unique<Server>(server_actor_id, KeyIntervals{}, *proxy_server);

		ASSERT_NO_THROW(runtime->register_actor(server.get()));

		discovery = std::make_unique<Discovery> (server_actor_id);
	}
	void TearDown() override {
		runtime.reset();
		proxy_server.reset();
		server.reset();
		discovery.reset();
	}

	std::shared_ptr<Runtime> runtime;
	common::GeneratorActorID builder;
	Clock clock;

	sdb::tx::ActorID server_actor_id;
	std::unique_ptr<ProxyRuntime> proxy_server;

	std::unique_ptr<Server> server;
	std::unique_ptr<Discovery> discovery;
	client::TxSpec spec{};
};

TEST_F(RuntimeTxStartMsgFixture, SimpleSendMsgFromActor) {
	auto actor1 = common::FakeActor(builder);
	ASSERT_NO_THROW(runtime->register_actor(&actor1));

	auto proxy_client = ProxyRuntime(runtime, actor1.get_actor_id());

	client::Client client(actor1.get_actor_id(), {spec}, discovery.get(), proxy_client);

	Messages msgs;

	const TxID txid = Generator::get_current_tx_id() + 1;
	const auto msg_id = Generator::get_next_msg_id();
	const auto expected_msg_id_ack = msg_id + 2;
	const auto ts = clock.next();
	const auto expected_ts_ack = ts+1;

	msgs.push_back(msg::Message{
			.type=msg::MessageType::MSG_UNDEFINED,
			.source=actor1.get_actor_id(),
			.destination=server_actor_id,
			.msg_id=msg_id,
	});

	client.send_on_tick(clock, std::move(msgs));

	runtime->run();

	const auto actual_mag = actor1.total.front();
	const auto expected_msg = msg::Message{
			.type=msg::MessageType::MSG_START_ACK,
			.source=server_actor_id,
			.destination=actor1.get_actor_id(),
			.msg_id=expected_msg_id_ack,
			.payload=msg::MsgPayload{
					.payload=msg::MsgAckStartPayload{
							.txid=txid,
							.read_ts=expected_ts_ack
					}
			}
	};

	ASSERT_TRUE(LightCmpMsg(actual_mag, expected_msg));
	ASSERT_EQ(actual_mag.payload.get<msg::MsgAckStartPayload>().txid,
			  expected_msg.payload.get<msg::MsgAckStartPayload>().txid);
}

TEST_F(RuntimeTxStartMsgFixture, SimpleSendMsgFromSomeActors) {
	auto actor1 = common::FakeActor(builder);
	auto actor2 = common::FakeActor(builder);

	ASSERT_NO_THROW(runtime->register_actor(&actor1));
	ASSERT_NO_THROW(runtime->register_actor(&actor2));

	auto proxy_client1 = ProxyRuntime(runtime, actor1.get_actor_id());
	client::Client client1(actor1.get_actor_id(), {spec}, discovery.get(), proxy_client1);

	auto proxy_client2 = ProxyRuntime(runtime, actor2.get_actor_id());
	client::Client client2(actor2.get_actor_id(), {spec}, discovery.get(), proxy_client2);

	{
		Messages msgs1;
		auto msg = msg::Message{
				.type=msg::MessageType::MSG_UNDEFINED,
				.source=actor1.get_actor_id(),
				.destination=server_actor_id,
				.msg_id=Generator::get_next_msg_id(),
		};
		msgs1.push_back(msg);

		client1.send_on_tick(clock, std::move(msgs1));
	}
	{
		Messages msgs2;
		auto msg = msg::Message{
				.type=msg::MessageType::MSG_UNDEFINED,
				.source=actor2.get_actor_id(),
				.destination=server_actor_id,
				.msg_id=Generator::get_next_msg_id(),
		};
		msgs2.push_back(msg);

		client2.send_on_tick(clock, std::move(msgs2));
	}

	runtime->run();

	ASSERT_TRUE(actor1.total.size() == 1);
	const auto actual_msg1 = actor1.total.front();
	ASSERT_EQ(actual_msg1.type, msg::MessageType::MSG_START_ACK);
	ASSERT_EQ(actual_msg1.source, server_actor_id);
	ASSERT_EQ(actual_msg1.destination, actor1.get_actor_id());
	ASSERT_EQ(actual_msg1.payload.get<msg::MsgAckStartPayload>().txid,
			  Generator::get_current_tx_id() - 1);

	ASSERT_TRUE(actor2.total.size() == 1);
	const auto actual_msg2 = actor2.total.front();
	ASSERT_EQ(actual_msg2.type, msg::MessageType::MSG_START_ACK);
	ASSERT_EQ(actual_msg2.source, server_actor_id);
	ASSERT_EQ(actual_msg2.destination, actor2.get_actor_id());
	ASSERT_EQ(actual_msg2.payload.get<msg::MsgAckStartPayload>().txid,
			  Generator::get_current_tx_id());
}