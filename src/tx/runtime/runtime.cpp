//
// Created by focus on 2/10/24.
//

#include "runtime.h"

#include <cassert>
#include <stdexcept>
#include <random>

#include <common/log/log.h>

namespace sdb::tx {

void Runtime::register_actor(IActor *actor) {
	assert(actor);

	const auto id = actor->get_actor_id();
	if (auto [_, status] = destination_actor_messages_.emplace(id, Messages{}); !status) {
		throw std::invalid_argument("failed register new actor");
	}
	actors_.push_back(actor);

	LOG_INFO << "Actor with id=" << id << " successfully registered.";
}

void Runtime::send(Message msg) {
	auto sent_msg = SentMessage{
			.send_timestamp=timestamp_,
			.delivery_timestamp=gen_.generate(1, 20),
			.msg=msg
	};
	LOG_DEBUG << "[Runtime::send] " << sent_msg;

	messages_queue_.push(sent_msg);
}

void Runtime::run(const int ticks) {
	for(int i=0; i<ticks; i++) {
		clear_destination_actor_messages_();

		timestamp_ = clock_.current();
		fill_destination_actor_messages();
		send_to_actors();

		if (clock_.current() == timestamp_) {
			timestamp_ = clock_.next();
//			LOG_DEBUG << "runtime with new ts=" << timestamp_;
		}
	}
}

void Runtime::clear_destination_actor_messages_() {
	for(auto& [actorID, messages] : destination_actor_messages_) {
		if (!messages.empty()) {
			LOG_DEBUG << "Clear messages for actor_id=" << actorID;
			messages.clear();
		}
	}
}

void Runtime::fill_destination_actor_messages() {
	while(!messages_queue_.empty() &&
		messages_queue_.top().delivery_timestamp <= timestamp_) {
		auto sent_msg = messages_queue_.top();
		messages_queue_.pop();

		LOG_DEBUG << "Take top msg from messages_queue. " << sent_msg;

		auto it = destination_actor_messages_.find(sent_msg.msg.destination);
		if (it == destination_actor_messages_.end()) {
			throw std::runtime_error("no destination");
		}

		it->second.push_back(sent_msg.msg);
	}
}

void Runtime::send_to_actors() {
	for(auto actor : actors_) {
		const auto destination = actor->get_actor_id();
		auto msgs = std::move(destination_actor_messages_[destination]);
		if (!msgs.empty()) {
			LOG_DEBUG << "Send " << msgs.size() << " msgs to actor " << destination;
			actor->send_on_tick(clock_, std::move(msgs));
		}
	}
}

} // namespace sdb::tx