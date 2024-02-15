//
// Created by focus on 2/15/24.
//

#include <gtest/gtest.h>

#include <tx/msg/message.h>

using namespace sdb::tx::msg;

TEST(TestCompareMsg, CmpSelf) {
	sdb::tx::msg::Message msg1;
	msg1.type = MessageType::MSG_START;
	msg1.source = 1;
	msg1.destination = 1;
	msg1.msg_id = 1;

	ASSERT_EQ(msg1, msg1);
}

TEST(TestCompareMsg, CmpEqualNoPayload) {
	sdb::tx::msg::Message msg1;
	msg1.type = MessageType::MSG_START;
	msg1.source = 1;
	msg1.destination = 1;
	msg1.msg_id = 1;

	sdb::tx::msg::Message msg2;
	msg2.type = MessageType::MSG_START;
	msg2.source = 1;
	msg2.destination = 1;
	msg2.msg_id = 1;

	ASSERT_EQ(msg1, msg2);
	ASSERT_EQ(msg2, msg1);
}

TEST(TestCompareMsg, ChangedDestinationFailed) {
	sdb::tx::msg::Message msg1;
	msg1.type = MessageType::MSG_START;
	msg1.source = 1;
	msg1.destination = 1;
	msg1.msg_id = 1;

	{
		auto msg_another = msg1;
		msg_another.destination++;
		ASSERT_FALSE(msg1 == msg_another);
	}
}

TEST(TestCompareMsg, ChangedSourceFailed) {
	sdb::tx::msg::Message msg1;
	msg1.type = MessageType::MSG_START;
	msg1.source = 1;
	msg1.destination = 1;
	msg1.msg_id = 1;

	{
		auto msg_another = msg1;
		msg_another.source++;
		ASSERT_FALSE(msg1 == msg_another);
	}
}

TEST(TestCompareMsg, CompareMsgIDFailed) {
	sdb::tx::msg::Message msg1;
	msg1.type = MessageType::MSG_START;
	msg1.source = 1;
	msg1.destination = 1;
	msg1.msg_id = 1;

	{
		auto msg_another = msg1;
		msg_another.msg_id++;
		ASSERT_FALSE(msg1 == msg_another);
	}
}

TEST(TestCompareMsg, CompareTypeFailed) {
	sdb::tx::msg::Message msg1;
	msg1.type = MessageType::MSG_START;
	msg1.source = 1;
	msg1.destination = 1;
	msg1.msg_id = 1;

	{
		auto msg_another = msg1;
		msg_another.type = MessageType::MSG_UNDEFINED;
		ASSERT_FALSE(msg1 == msg_another);
	}
}

TEST(TestCompareMsg, CmpMsgStartPayloadFailed) {
	sdb::tx::msg::Message msg1;
	msg1.type = sdb::tx::msg::MessageType::MSG_START;
	msg1.source = 11;
	msg1.destination = 223;
	msg1.msg_id = 1234;

	msg1.payload.payload = MsgStartPayload{
		.txid=10517,
		.ts=1'000'251,
	};

	{
		auto msg_another = msg1;
		msg_another.payload.get<MsgStartPayload>().txid = 255;

		ASSERT_FALSE(msg1 == msg_another);
	}
	{
		auto msg_another = msg1;
		msg_another.payload.get<MsgStartPayload>().ts = 355;

		ASSERT_FALSE(msg1 == msg_another);
	}

	{
		auto msg_another = msg1;
		msg_another.payload.payload = MsgStartPayload{
				.txid=1,
				.ts=1,
		};
		ASSERT_FALSE(msg1 == msg_another);
	}
}