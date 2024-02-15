//
// Created by focus on 2/15/24.
//

#include <gtest/gtest.h>

#include <tx/runtime/runtime.h>
#include <tx/runtime/proxy_runtime.h>
#include <tx/server/server.h>
#include <tx/clock/clock.h>

#include "common/fake_actor.h"


TEST(TestServerTX, SimpleSendMsg) {
	auto runtime = std::make_shared<sdb::tx::Runtime>();

	common::GeneratorActorID builder;

	auto actor1 = common::FakeActor(builder);

	auto proxy_actor1 = sdb::tx::ProxyRuntime(runtime, actor1.get_actor_id());

	const auto server_actor_id = builder();

	sdb::tx::Server server(server_actor_id, {}, proxy_actor1);

	sdb::tx::Clock clock;

	sdb::tx::Messages msgs;
	msgs.push_back(sdb::tx::msg::Message{
		.type=sdb::tx::msg::MessageType::MSG_START,
		.source=server_actor_id,
		.destination=actor1.get_actor_id(),
		.msg_id=1,
		.payload=sdb::tx::msg::MsgPayload{
			.payload=sdb::tx::msg::MsgStartPayload{
				.txid=1,
				.ts=1
			}
		},
	});

	server.send_on_tick(clock, std::move(msgs));

	ASSERT_EQ(actor1.total.size(), 1);
}