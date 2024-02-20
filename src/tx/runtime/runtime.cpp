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

	const auto actor_id = actor->get_actor_id();
	if (auto [_, status] = destination_actor_messages_.emplace(actor_id, Messages{}); !status) {
		throw std::invalid_argument("failed register new actor");
	}
	actors_.push_back(actor);

	LOG_INFO << "Actor_id=" << actor_id << " successfully registered.";
}

void Runtime::send(msg::Message msg) {
	const auto current_clock = clock_.current();
	auto sent_msg = msg::SentMessage{
			.send_timestamp=timestamp_,
			.delivery_timestamp=current_clock, // gen_.generate(current_clock, current_clock+1),
			.msg=msg
	};
	LOG_DEBUG << "[Runtime::send][ts=" << current_clock <<"] "
		<< sent_msg << " add to messages_queue.";

	messages_queue_.push(sent_msg);
}

void Runtime::run(const int ticks) {
	for(int i=0; i<ticks; i++) {
		LOG_DEBUG << "[Runtime::run] start tick=" << i;
		clear_destination_actor_messages();

		timestamp_ = clock_.current();
		LOG_DEBUG << "[Runtime::run] set ts=" << timestamp_;
		fill_destination_actor_messages();
		send_to_actors();

		if (clock_.current() == timestamp_) {
			timestamp_ = clock_.next();
//			LOG_DEBUG << "runtime with new ts=" << timestamp_;
		}
	}
}

void Runtime::clear_destination_actor_messages() {
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

		LOG_DEBUG << "[Runtime] Take top msg from messages_queue. " << sent_msg;

		auto it = destination_actor_messages_.find(sent_msg.msg.destination);
		if (it == destination_actor_messages_.end()) {
			throw std::runtime_error("no destination");
		}

		it->second.push_back(sent_msg.msg);
	}
}

void Runtime::send_to_actors() {
	LOG_DEBUG << "[Runtime::send_to_actors] call. actors size=" << actors_.size();
	for(auto actor : actors_) {
		const auto destination = actor->get_actor_id();
		auto msgs = std::move(destination_actor_messages_[destination]);

		LOG_DEBUG << "[Runtime::send_to_actors] Send " << msgs.size() << " msgs to actor_id= " << destination;
		actor->send_on_tick(clock_, std::move(msgs));
	}
}

} // namespace sdb::tx