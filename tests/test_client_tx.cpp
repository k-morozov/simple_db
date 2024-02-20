//
// Created by focus on 2/20/24.
//


#include <gtest/gtest.h>

#include <tx/generator/generator.h>
#include <tx/discovery/discovery.h>
#include <tx/retrier/retrier.h>
#include <tx/client/client_tx.h>
#include <tx/msg/message.h>

using namespace sdb::tx;
using Message = msg::Message;

TEST(TestClientTx, SimplePut) {
	const sdb::tx::ActorID node_id = 1;
	const sdb::tx::ActorID client = 10;

	client::TxSpec spec{
			1, // start timestamp
			4, // commit timestamp
			{{
					 // Get
					 2, // start timestamp
					 1, // key
			 }},
			{{
					 // Put
					 3,  // start timestamp
					 2,  // key
					 10, // value
			 }},
			client::TxSpec::Action::COMMIT
	};

	sdb::tx::Discovery discovery(node_id);
	Retrier rt;

	client::ClientTx tx(client, spec, &discovery, &rt);

	std::vector<Message> in;
	std::vector<Message> out;
	tx.tick(1, in, &out);

	ASSERT_EQ(out.size(), 1);
	ASSERT_EQ(out[0].type, msg::MessageType::MSG_START);
	ASSERT_EQ(out[0].destination, node_id);

	const sdb::tx::TxID txid = get_txid_from_msg_payload(out[0]);

	in.push_back(msg::CreateMsgStartAck(node_id, client, txid, 1));
	out.clear();

	tx.tick(2, in, &out);
	ASSERT_TRUE(out.empty());
	in.clear();

	tx.tick(3, in, &out);

	ASSERT_EQ(out.size(), 1);

	ASSERT_EQ(out[0].type, msg::MessageType::MSG_PUT);
	ASSERT_EQ(out[0].payload.get<msg::MsgPutPayload>().txid, txid);
	ASSERT_EQ(out[0].payload.get<msg::MsgPutPayload>().key, 2);
	ASSERT_EQ(out[0].payload.get<msg::MsgPutPayload>().value, 10);

	in.push_back(msg::CreateMsgPutReply(node_id, client, txid, out[0].msg_id));
	out.clear();
	tx.tick(4, in, &out);

	ASSERT_EQ(out.size(), 1);
	ASSERT_EQ(out[0].type, msg::MessageType::MSG_COMMIT);
	ASSERT_EQ(out[0].payload.get<msg::MsgCommitPayload>().txid, txid);

	in.clear();
	out.clear();
	in.push_back(msg::CreateMsgCommitAck(node_id, client, txid, 5));
	tx.tick(5, in, &out);

	ASSERT_TRUE(out.empty());

	ASSERT_EQ(tx.get_state(), sdb::tx::client::ClientTXState::COMMITTED);

	auto res = tx.export_results();

	ASSERT_EQ(res.read_ts, 1);
	ASSERT_EQ(res.commit_ts, 5);

	//	assert(res.gets[0].key == 1);
	//	assert(res.gets[0].value == 250);

	ASSERT_EQ(res.puts[0].first, 2);
	ASSERT_EQ(res.puts[0].second, 10);
}