//
// Created by focus on 2/15/24.
//

#include "client_tx.h"

#include <stdexcept>
#include <unordered_map>

#include <common/log/log.h>
#include <tx/msg/message.h>

namespace sdb::tx::client {

std::string to_string(const ClientTXState state) {
	switch (state) {
		case ClientTXState::NOT_STARTED:
			return "NOT_STARTED";
		case ClientTXState::START_SENT:
			return "START_SENT";
		case ClientTXState::OPEN:
			return "OPEN";
	}
	throw std::runtime_error("broken ClientTXState for switch");
}

ClientTx::ClientTx(ActorID actor_id, const ClientTxSpec &spec, Retrier *retrier) :
	actor_id_(actor_id),
	spec_(spec),
	retrier_(retrier)
{
	LOG_DEBUG << "[ClientTx][actor_id=" << actor_id_ << "] created.";
}

void ClientTx::tick(const Timestamp ts,
					const Messages &msgs,
					Messages *msg_out) {
	Messages msg_start_tx;
	std::unordered_map<ActorID, Messages> reply_messages_per_actor;
	
	for(const auto& msg : msgs) {
		switch (msg.type) {
			case msg::MessageType::MSG_START_ACK:
				LOG_DEBUG << "[ClientTx::tick] got reply msg from server: " << msg;
				reply_messages_per_actor[msg.source].push_back(msg);
				break;
			default:
				LOG_DEBUG << "[ClientTx::tick] got new msg from server: " << msg;
				msg_start_tx.push_back(msg);
				break;
		}
	}

	switch (state_) {
		case ClientTXState::NOT_STARTED: {
			// @todo process msgs

			if (ts >= spec_.earliest_start_ts) {
				LOG_DEBUG << "[ClientTx::tick] ts=" << ts
					<< ", spec: " << spec_;

				// @todo coordinator

				LOG_DEBUG << "[ClientTx::tick] change state from " << to_string(state_)
					<< " to " << to_string(ClientTXState::START_SENT);
				state_ = ClientTXState::START_SENT;
			}
		}
			break;
	}

	retrier_->get_ready(ts, msg_out);
}

} // namespace sdb::tx::client