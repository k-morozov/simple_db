//
// Created by focus on 2/15/24.
//

#include <gtest/gtest.h>

#include <tx/runtime/runtime.h>
#include <tx/runtime/proxy_runtime.h>
#include <tx/server/server.h>
#include <tx/clock/clock.h>
#include <tx/msg/generator_msg_id.h>

#include "common/fake_actor.h"

constexpr int DefaultTicksCount = 25;

class RuntimeTxStartMsgFixture : public ::testing::Test {
public:
	void SetUp() override {
		runtime = std::make_shared<sdb::tx::Runtime>();
	}
	void TearDown() override {
		runtime.reset();
	}

	std::shared_ptr<sdb::tx::Runtime> runtime;
	common::GeneratorActorID builder;
	sdb::tx::Clock clock;
};

TEST_F(RuntimeTxStartMsgFixture, SimpleSendMsg) {
	auto actor1 = common::FakeActor(builder);

	const auto server_actor_id = builder();
	auto proxy_server = sdb::tx::ProxyRuntime(runtime, server_actor_id);

	sdb::tx::Server server(server_actor_id, {}, proxy_server);

	ASSERT_NO_THROW(runtime->register_actor(&actor1));
	ASSERT_NO_THROW(runtime->register_actor(&server));

	sdb::tx::Messages msgs;

	const sdb::tx::TxID txid = 1;
	const auto msg_id = sdb::tx::msg::GeneratorMsgID::get_next_id();
	const auto expected_msg_id_ack = msg_id + 1;
	const auto ts = clock.next();
	const auto expected_ts_ack = ts+1;

	msgs.push_back(sdb::tx::msg::Message{
		.type=sdb::tx::msg::MessageType::MSG_START,
		.source=actor1.get_actor_id(),
		.destination=server_actor_id,
		.msg_id=msg_id,
		.payload=sdb::tx::msg::MsgPayload{
			.payload=sdb::tx::msg::MsgStartPayload{
				.txid=txid,
				.ts=ts
			}
		},
	});

	server.send_on_tick(clock, std::move(msgs));

	runtime->run(DefaultTicksCount);

	ASSERT_EQ(actor1.total.size(), 1);

	const auto actual_mag = actor1.total.front();
	const auto expected_msg = sdb::tx::msg::Message{
		.type=sdb::tx::msg::MessageType::MSG_START_ACK,
		.source=server_actor_id,
		.destination=actor1.get_actor_id(),
		.msg_id=expected_msg_id_ack,
		.payload=sdb::tx::msg::MsgPayload{
			.payload=sdb::tx::msg::MsgAckStartPayload{
				.txid=txid,
				.read_ts=expected_ts_ack
			}
		}
	};

	ASSERT_EQ(actual_mag, expected_msg);
}