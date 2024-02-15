//
// Created by focus on 2/10/24.
//

#pragma once

#include <ostream>

#include <tx/msg/message.h>
#include <tx/types.h>

namespace sdb::tx::msg {

struct SentMessage final {
	Timestamp send_timestamp{};
	Timestamp delivery_timestamp{};
	Message msg;
};

struct DeliveredLast {
	bool operator()(const SentMessage& lhs, const SentMessage& rhs) const;
};

std::ostream& operator<<(std::ostream& stream, const SentMessage& sm);

} // namespace sdb::tx::msg
