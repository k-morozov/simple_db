//
// Created by focus on 2/15/24.
//

#include "client_tx.h"

#include <stdexcept>
#include <unordered_map>

#include <common/log/log.h>
#include <tx/msg/message.h>
#include <cassert>

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

void progress_state(ClientTXState* state) {
	assert(state);

	switch (*state) {
		case ClientTXState::NOT_STARTED:
			*state = ClientTXState::START_SENT;
			break;
		case ClientTXState::START_SENT:
		case ClientTXState::OPEN:
			throw std::invalid_argument("have not implemented yet.");
	}
}

std::ostream& operator<<(std::ostream& stream, const ClientTx& self) {
	stream << "[ClientTx]"
		   << "[actor_id_=" << self.actor_id_ << "]"
		   << self.spec_
		   << "[state_=" << to_string(self.state_) << "]"
		   << "[coordinator=" << self.coordinator_actor_id_ << "]";

	return stream;
}

ClientTx::ClientTx(ActorID actor_id, const TxSpec& spec, const Discovery* discovery, Retrier *retrier) :
	actor_id_(actor_id),
	spec_(spec),
	discovery_(discovery),
	retrier_(retrier)
{
	LOG_SELF_DEBUG << "created.";
}

void ClientTx::tick(const Timestamp ts,
					const Messages& msgs,
					Messages* msg_out) {
	Messages msg_start_tx;
	std::unordered_map<ActorID, Messages> reply_messages_per_actor;
	
	for(const auto& msg : msgs) {
		switch (msg.type) {
			case msg::MessageType::MSG_START_ACK:
				LOG_SELF_DEBUG << "got reply msg from server: " << msg;
				reply_messages_per_actor[msg.source].push_back(msg);
				break;
			default:
				LOG_SELF_DEBUG << "got new msg from server: " << msg;
				msg_start_tx.push_back(msg);
				break;
		}
	}

	switch (state_) {
		case ClientTXState::NOT_STARTED: {
			assert(msgs.empty());

			// we started work with tx not before earliest_start_ts in tx spec.
			if (ts >= spec_.earliest_start_ts) {
				LOG_DEBUG << "[ClientTx::tick] ts=" << ts << " is greater/equal than earliest_start_ts=" << spec_.earliest_start_ts
					<< ", configure coordinator.";

				configure_coordinator(ts);

				LOG_DEBUG << "[ClientTx::tick] change state from " << to_string(state_)
					<< " to " << to_string(ClientTXState::START_SENT);

				progress_state(&state_);
			}
		}
			break;
	}

	retrier_->get_ready(ts, msg_out);
}

void ClientTx::configure_coordinator(const Timestamp ts) {
	coordinator_actor_id_ = discovery_->get_random();
	LOG_DEBUG << "[ClientTx::tick] setup coordinator=" << coordinator_actor_id_;

	participants_[coordinator_actor_id_] = std::make_unique<TxParticipant>(
			get_actor_id(), coordinator_actor_id_, retrier_);

	participants_[coordinator_actor_id_]->start(ts);

	txid_ = participants_[coordinator_actor_id_]->txid();
}

} // namespace sdb::tx::client