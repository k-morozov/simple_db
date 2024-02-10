//
// Created by focus on 2/10/24.
//

#include "runtime.h"

#include <cassert>
#include <stdexcept>

#include <log/log.h>

namespace sdb::tx {

void Runtime::register_actor(IActor *actor) {
	assert(actor);

	const auto id = actor->get_actor_id();
	if (auto [_, status] = actor_messages_.emplace(id, Messages{}); !status) {
		throw std::invalid_argument("failed register new actor");
	}
	actors_.push_back(actor);

	LOG_INFO << "actor with id=" << id << " successfully registered.";
}

void Runtime::send(Message&& msg) {
	LOG_DEBUG << "[ts " << timestamp_ << "] Send msg with type=" << msg.type
		<< " from " << msg.source << " to " << msg.destination;

	SentMessage sent_msg(timestamp_, msg);

	sent_msg.delivery_timestamp = timestamp_ + 10;
	// @todo why generate_delivery_delay?
	messages_queue_.push(sent_msg);
}

void Runtime::run(const int ticks) {
	for(int i=0; i<ticks; i++) {
		clear_actor_messages();

		timestamp_ = clock_.current();
		fill_actor_messages();
		send_to_actors();

		if (clock_.current() == timestamp_) {
			timestamp_ = clock_.next();
		}
	}
}

void Runtime::clear_actor_messages() {
	for(auto& [actorID, messages] : actor_messages_) {
		messages.clear();
	}
}

void Runtime::fill_actor_messages() {
	while(!messages_queue_.empty() &&
		messages_queue_.top().delivery_timestamp <= timestamp_) {
		auto sent_msg = messages_queue_.top();
		messages_queue_.pop();

		auto it = actor_messages_.find(sent_msg.msg.destination);
		if (it == actor_messages_.end()) {
			throw std::runtime_error("no destination");
		}
		it->second.push_back(sent_msg.msg);
	}
}

void Runtime::send_to_actors() {
	for(auto actor : actors_) {
		actor->send_on_tick(std::move(actor_messages_[actor->get_actor_id()]));
	}
}

} // namespace sdb::tx