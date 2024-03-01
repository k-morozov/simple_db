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

TEST(TestClientTx, SimpleConflictedPuts) {
	const sdb::tx::ActorID node_id = 1;
	const sdb::tx::ActorID client = 10;

	client::TxSpec spec1{
			1, // start timestamp
			4, // commit timestamp
			{},
			{{
				// Put
				3,  // start timestamp
				1,  // key
				117, // value
			 }},
			client::TxSpec::Action::COMMIT
	};

	client::TxSpec spec2{
			1, // start timestamp
			3, // commit timestamp
			{},
			{{
				// Put
				3,  // start timestamp
				2,  // key
				217, // value
			 }},
			client::TxSpec::Action::COMMIT
	};

	sdb::tx::Discovery discovery(node_id);
	Retrier rt;

	client::ClientTx tx1(client, spec1, &discovery, &rt);
	client::ClientTx tx2(client, spec2, &discovery, &rt);

	std::vector<Message> in1;
	std::vector<Message> out1;
	tx1.tick(1, in1, &out1);

	ASSERT_EQ(out1.size(), 1);
	ASSERT_EQ(out1[0].type, msg::MessageType::MSG_START);
	ASSERT_EQ(out1[0].destination, node_id);

	std::vector<Message> in2;
	std::vector<Message> out2;
	tx2.tick(2, in2, &out2);

	const sdb::tx::TxID txid1 = get_txid_from_msg_payload(out1[0]);
	const sdb::tx::TxID txid2 = get_txid_from_msg_payload(out2[0]);

	{ // send put for tx1
		in1.push_back(msg::CreateMsgStartAck(node_id, client, txid1, 1));
		out1.clear();

		tx1.tick(3, in1, &out1);

		ASSERT_TRUE(out1.empty());
		in1.clear();
		tx1.tick(4, in1, &out1);
		ASSERT_EQ(out1.size(), 1);
		ASSERT_EQ(out1[0].type, msg::MessageType::MSG_PUT);
		ASSERT_EQ(out1[0].payload.get<msg::MsgPutPayload>().txid, txid1);
		ASSERT_EQ(out1[0].payload.get<msg::MsgPutPayload>().key, 1);
		ASSERT_EQ(out1[0].payload.get<msg::MsgPutPayload>().value, 117);
	}

	{ // send put for tx2
		in2.push_back(msg::CreateMsgStartAck(node_id, client, txid2, 1));
		out2.clear();

		tx2.tick(5, in2, &out2);

		ASSERT_TRUE(out2.empty());
		in2.clear();
		tx2.tick(6, in2, &out2);
		ASSERT_EQ(out2.size(), 1);
		ASSERT_EQ(out2[0].type, msg::MessageType::MSG_PUT);
		ASSERT_EQ(out2[0].payload.get<msg::MsgPutPayload>().txid, txid2);
		ASSERT_EQ(out2[0].payload.get<msg::MsgPutPayload>().key, 2);
		ASSERT_EQ(out2[0].payload.get<msg::MsgPutPayload>().value, 217);
	}

	{ // send put reply for tx1
		in1.push_back(msg::CreateMsgPutReply(node_id, client, txid1, out1[0].msg_id));
		out1.clear();
		tx1.tick(7, in1, &out1);

		ASSERT_EQ(out1.size(), 1);
		ASSERT_EQ(out1[0].type, msg::MessageType::MSG_COMMIT);
		ASSERT_EQ(out1[0].payload.get<msg::MsgCommitPayload>().txid, txid1);
	}

	{ // send put reply for tx2
		in2.push_back(msg::CreateMsgPutReply(node_id, client, txid2, out2[0].msg_id));
		out2.clear();
		tx2.tick(8, in2, &out2);

		ASSERT_EQ(out2.size(), 1);
		ASSERT_EQ(out2[0].type, msg::MessageType::MSG_COMMIT);
		ASSERT_EQ(out2[0].payload.get<msg::MsgCommitPayload>().txid, txid2);
	}

	{ // send commit ack for tx1
		in1.clear();
		out1.clear();
		in1.push_back(msg::CreateMsgCommitAck(node_id, client, txid1, 9));
		tx1.tick(10, in1, &out1);

		ASSERT_TRUE(out1.empty());

		ASSERT_EQ(tx1.get_state(), sdb::tx::client::ClientTXState::COMMITTED);
	}

	{ // send commit ack for tx2
		in2.clear();
		out2.clear();
		in2.push_back(msg::CreateMsgCommitAck(node_id, client, txid1, 11));
		tx2.tick(12, in2, &out2);

		ASSERT_TRUE(out2.empty());

		ASSERT_EQ(tx2.get_state(), sdb::tx::client::ClientTXState::COMMITTED);
	}

	{ // check tx1
		auto res = tx1.export_results();
		ASSERT_EQ(res.read_ts, 1);
		ASSERT_EQ(res.commit_ts, 9);
		ASSERT_EQ(res.puts[0].first, 1);
		ASSERT_EQ(res.puts[0].second, 117);
	}

	{ // check tx2
		auto res = tx2.export_results();
		ASSERT_EQ(res.read_ts, 1);
		ASSERT_EQ(res.commit_ts, 11);
		ASSERT_EQ(res.puts[0].first, 2);
		ASSERT_EQ(res.puts[0].second, 217);
	}
}