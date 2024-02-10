//
// Created by focus on 2/10/24.
//

#include "runtime.h"

#include <cassert>
#include <stdexcept>

#include <log/log.h>

namespace sdb::actor {

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

	// @todo why generate_delivery_delay?
	messages_queue_.push(sent_msg);
}
} // namespace sdb::actor