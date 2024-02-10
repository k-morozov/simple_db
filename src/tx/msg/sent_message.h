//
// Created by focus on 2/10/24.
//

#pragma once

#include <optional>

#include <tx/msg/message.h>
#include <tx/types.h>

namespace sdb::tx {

struct SentMessage final {
	SentMessage(const Timestamp send_timestamp, const Message msg):
		send_timestamp(send_timestamp),
		msg(msg)
	{};

	SentMessage(const SentMessage& sm) = default;
	SentMessage& operator=(const SentMessage& sm) = default;

	Timestamp send_timestamp;
	std::optional<Timestamp> delivery_timestamp;
	Message msg;
};

struct DeliveredLast {
	bool operator()(const SentMessage& lhs, const SentMessage& rhs) const;
};

} // namespace sdb::tx
