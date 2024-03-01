//
// Created by focus on 2/15/24.
//

#include "retrier.h"

#include <cassert>

#include <tx/msg/message.h>
#include <common/log/log.h>

namespace sdb::tx {

void Retrier::send_once([[maybe_unused]] const Timestamp ts,
						const msg::Message msg) {
	schedule_msgs_.push_back(msg);
}

Retrier::Handle Retrier::schedule(const Timestamp ts, const msg::Message msg) {
	send_once(ts, msg);
	return -1;
}

void Retrier::get_scheduled_msgs(const Timestamp ts,
								 Messages* messages) {
	assert(messages);
	LOG_DEBUG << "[Retrier::get_scheduled_msgs][ts=" << ts << "] call with " << messages->size() << " messages";

	messages->reserve(messages->size() + schedule_msgs_.size());

	for(auto& msg : schedule_msgs_) {
		messages->push_back(msg);
	}
	schedule_msgs_.clear();
}

} // namespace sdb::tx