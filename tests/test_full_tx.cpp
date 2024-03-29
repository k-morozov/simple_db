//
// Created by focus on 2/15/24.
//

#include <gtest/gtest.h>
#include <tx/client/client.h>
#include <tx/client/tx_spec.h>
#include <tx/clock/clock.h>
#include <tx/discovery/discovery.h>
#include <tx/generator/generator.h>
#include <tx/runtime/proxy_runtime.h>
#include <tx/runtime/runtime.h>
#include <tx/server/server.h>

#include <memory>

#include "common/fake_actor.h"

using namespace sdb::tx;
using Message = msg::Message;

bool LightCmpMsg(const Message &lhs, const Message &rhs) {
    if (lhs.type != rhs.type) return false;
    if (lhs.source != rhs.source) return false;
    if (lhs.destination != rhs.destination) return false;
    if (lhs.msg_id != rhs.msg_id) return false;
    if (lhs.payload.payload.index() != rhs.payload.payload.index()) return false;

    return true;
}

class TxFixture : public ::testing::Test {
public:
    void SetUp() override {
        runtime = std::make_shared<Runtime>();
        server_actor_id = builder();
        proxy_server = std::make_unique<ProxyRuntime>(runtime, server_actor_id);
        server = std::make_unique<server::Server>(server_actor_id, KeyIntervals{},
                                                  *proxy_server);

        ASSERT_NO_THROW(runtime->register_actor(server.get()));

        discovery = std::make_unique<Discovery>(server_actor_id);
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

    std::unique_ptr<server::Server> server;
    std::unique_ptr<Discovery> discovery;
};

TEST_F(TxFixture, SimpleSendMsgStartFromActor) {
    const auto test_client_actor_id = builder();
    auto proxy_client = ProxyRuntime(runtime, test_client_actor_id);

    client::TxSpec spec{
            .earliest_start_ts = 0,
            .earliest_commit_ts = 2,
            .gets = {},
            .puts = {client::ClientTxPut{.earliest_ts = 2, .key = 17, .value = 1177}},
            .action = client::TxSpec::Action::COMMIT};

    client::Client client(test_client_actor_id, {spec}, discovery.get(),
                          proxy_client);

    ASSERT_NO_THROW(runtime->register_actor(&client));

    runtime->run(10);

    const auto res = client.get_tx(0)->export_results();

    ASSERT_TRUE(res.read_ts >= 1);
    ASSERT_TRUE(res.commit_ts >= 2);
    ASSERT_EQ(res.txid, 1001);
    ASSERT_TRUE(res.puts.size() == 1);
    ASSERT_EQ(res.puts[0], client::ClientTx::ExportResult::KeyValue(17, 1177));
}

TEST_F(TxFixture, RollbackTxDifferentClients) {
    const auto test_client1_actor_id = builder();
    auto proxy_client1 = ProxyRuntime(runtime, test_client1_actor_id);

    client::TxSpec spec1{
            .earliest_start_ts = 0,
            .earliest_commit_ts = 2,
            .gets = {},
            .puts = {client::ClientTxPut{.earliest_ts = 2, .key = 17, .value = 1177}},
            .action = client::TxSpec::Action::COMMIT};

    const auto test_client2_actor_id = builder();
    auto proxy_client2 = ProxyRuntime(runtime, test_client2_actor_id);

    client::TxSpec spec2{
            .earliest_start_ts = 0,
            .earliest_commit_ts = 2,
            .gets = {},
            .puts = {client::ClientTxPut{.earliest_ts = 2, .key = 17, .value = 2277}},
            .action = client::TxSpec::Action::COMMIT};

    client::Client client1(test_client1_actor_id, {spec1}, discovery.get(),
                           proxy_client1);
    client::Client client2(test_client2_actor_id, {spec2}, discovery.get(),
                           proxy_client2);

    ASSERT_NO_THROW(runtime->register_actor(&client1));
    ASSERT_NO_THROW(runtime->register_actor(&client2));

    runtime->run(20);

    {
        const auto res1 = client1.get_tx(0)->export_results();
        ASSERT_EQ(res1.state, client::ClientTXState::ROLLED_BACK_BY_SERVER);
//        ASSERT_EQ(res1.txid, 1001);
        ASSERT_TRUE(res1.conflict_txid != UNDEFINED_TX_ID);
        ASSERT_TRUE(res1.read_ts >= 0);
        ASSERT_TRUE(res1.commit_ts == UNDEFINED_TS);
        ASSERT_TRUE(res1.puts.size() == 1);
    }

    {
        const auto res2 = client2.get_tx(0)->export_results();
        ASSERT_EQ(res2.state, client::ClientTXState::COMMITTED);
//        ASSERT_EQ(res2.txid, 1002);
        ASSERT_TRUE(res2.conflict_txid == UNDEFINED_TX_ID);
        ASSERT_TRUE(res2.read_ts >= 1);
        ASSERT_TRUE(res2.commit_ts >= 2);
        ASSERT_TRUE(res2.puts.size() == 1);
        ASSERT_EQ(res2.puts[0], client::ClientTx::ExportResult::KeyValue(17, 2277));
    }
}
