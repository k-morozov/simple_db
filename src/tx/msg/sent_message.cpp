//
// Created by focus on 2/10/24.
//

#pragma once

#include "sent_message.h"

#include <tuple>

namespace sdb::tx {

bool tx::DeliveredLast::operator()(const SentMessage &lhs, const SentMessage &rhs) const {
	return std::make_tuple(lhs.delivery_timestamp, lhs.send_timestamp)
	>
	std::make_tuple(rhs.delivery_timestamp, rhs.send_timestamp);
}

std::ostream& operator<<(std::ostream& stream, const SentMessage& sm) {
	stream << "message [type=Sent]"
			<< "[send_ts=" << sm.send_timestamp << "]"
			<< "[delivery_ts " << sm.delivery_timestamp << "]"
			<< " includes " << sm.msg;
	return stream;
}

} // namespace sdb::tx