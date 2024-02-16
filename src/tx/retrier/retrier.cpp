//
// Created by focus on 2/15/24.
//

#include "retrier.h"

#include <cassert>

#include <tx/msg/message.h>

namespace sdb::tx {

void Retrier::send_once([[maybe_unused]] const Timestamp ts,
						const msg::Message msg) {
	outgoing_.push_back(msg);
}

Retrier::Handle Retrier::schedule(const Timestamp ts, const msg::Message msg) {
	send_once(ts, msg);
	return -1;
}

void Retrier::get_ready(const Timestamp ts,
						Messages* messages) {
	assert(messages);
	messages->reserve(messages->size() + outgoing_.size());

	for(auto& msg : outgoing_) {
		messages->push_back(msg);
	}
	outgoing_.clear();
}

} // namespace sdb::tx