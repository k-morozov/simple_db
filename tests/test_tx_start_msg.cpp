//
// Created by focus on 2/15/24.
//

#include <gtest/gtest.h>

#include <tx/runtime/runtime.h>
#include <tx/runtime/proxy_runtime.h>
#include <tx/server/server.h>
#include <tx/clock/clock.h>
#include <tx/generator/generator.h>
#include <tx/discovery/discovery.h>
#include <tx/client/client.h>
#include <tx/client/client_tx_spec.h>

#include "common/fake_actor.h"

constexpr int DefaultTicksCount = 100;

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
	}
	void TearDown() override {
		runtime.reset();
	}

	std::shared_ptr<Runtime> runtime;
	common::GeneratorActorID builder;
	Clock clock;
};

TEST_F(RuntimeTxStartMsgFixture, SimpleSendMsg) {
	auto actor1 = common::FakeActor(builder);

	const auto server_actor_id = builder();
	auto proxy_server = ProxyRuntime(runtime, server_actor_id);

	Server server(server_actor_id, {}, proxy_server);

	ASSERT_NO_THROW(runtime->register_actor(&actor1));
	ASSERT_NO_THROW(runtime->register_actor(&server));

	Discovery discovery(server_actor_id);
	client::ClientTxSpec spec;
	auto proxy_client = ProxyRuntime(runtime, actor1.get_actor_id());

	client::Client client(actor1.get_actor_id(), {spec}, &discovery, proxy_client);

	Messages msgs;

	const TxID txid = 1;
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
							.txid=1001,
							.read_ts=expected_ts_ack
					}
			}
	};

	ASSERT_TRUE(LightCmpMsg(actual_mag, expected_msg));
	ASSERT_EQ(actual_mag.payload.get<msg::MsgAckStartPayload>().txid,
			  expected_msg.payload.get<msg::MsgAckStartPayload>().txid);
}