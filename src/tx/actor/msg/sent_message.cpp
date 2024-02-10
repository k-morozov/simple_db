//
// Created by focus on 2/10/24.
//

#pragma once

#include "sent_message.h"

#include <tuple>

namespace sdb::actor {

bool actor::DeliveredLast::operator()(const SentMessage &lhs, const SentMessage &rhs) const {
	return std::make_tuple(lhs.delivery_timestamp.value_or(0), lhs.send_timestamp)
	>
	std::make_tuple(rhs.delivery_timestamp.value_or(0), rhs.send_timestamp);
}

} // namespace sdb::actor