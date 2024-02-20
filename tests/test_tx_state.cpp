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

class TxStateFixture : public ::testing::Test {
public:
	void SetUp() override {
		runtime = std::make_shared<Runtime>();
		server_actor_id = builder();
		proxy_server = std::make_unique<ProxyRuntime>(runtime, server_actor_id);
		server = std::make_unique<common::TestServer>(server_actor_id, KeyIntervals{}, *proxy_server);

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

	sdb::tx::ActorID server_actor_id;
	std::unique_ptr<ProxyRuntime> proxy_server;

	std::unique_ptr<Server> server;
	std::unique_ptr<Discovery> discovery;
	client::TxSpec spec{
		.earliest_start_ts=0,
		.earliest_commit_ts=2,
		.gets={},
		.puts={
			client::ClientTxPut{
				.earliest_ts=2,
				.key=17,
				.value=1177
			}
		},
		.action=client::TxSpec::Action::COMMIT
	};
};

TEST_F(TxStateFixture, SimpleSendMsgStartFromActor) {
	const auto test_client_actor_id = builder();
	auto proxy_client = ProxyRuntime(runtime, test_client_actor_id);
	client::Client client(test_client_actor_id, {spec}, discovery.get(), proxy_client);

	ASSERT_NO_THROW(runtime->register_actor(&client));

	runtime->run();

	const auto res = client.get_tx(0)->export_results();

	ASSERT_EQ(res.read_ts, -1);
	ASSERT_TRUE(res.commit_ts >=2);
	ASSERT_EQ(res.txid, 1001);
	ASSERT_TRUE(res.puts.size() == 1);
	ASSERT_EQ(res.puts[0], client::ClientTx::ExportResult::KeyValue(17, 1177));

}
