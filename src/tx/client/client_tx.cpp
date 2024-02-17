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

std::ostream& operator<<(std::ostream& stream, const ClientTXState& state) {
	switch (state) {
		case ClientTXState::NOT_STARTED:
			stream << "NOT_STARTED";
			break;
		case ClientTXState::START_SENT:
			stream << "START_SENT";
			break;
		case ClientTXState::OPEN:
			stream << "OPEN";
			break;
	}
	return stream;
}

void progress_state(ClientTXState* state) {
	assert(state);

	switch (*state) {
		case ClientTXState::NOT_STARTED:
			LOG_DEBUG << "[ClientTx::tick]"
					  << " change state form NOT_STARTED to START_SENT";
			*state = ClientTXState::START_SENT;
			break;
		case ClientTXState::START_SENT:
			LOG_DEBUG << "[ClientTx::tick]"
					  << " change state form START_SENT to OPEN";
			*state = ClientTXState::OPEN;
			break;
		case ClientTXState::OPEN:
			throw std::invalid_argument("have not implemented yet.");
	}
}

std::ostream& operator<<(std::ostream& stream, const ClientTx& self) {
	stream << "[ClientTx]"
		   << "[actor_id_=" << self.actor_id_ << "]"
		   << self.spec_
		   << "[state_=" << self.state_ << "]"
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

	for(auto& [actor_id, participant] : participants_) {
		participant->process_incoming(ts, reply_messages_per_actor[actor_id]);
	}

	LOG_DEBUG << "[ClientTx::tick][state=" << state_ << "][ts=" << ts << "]";
	switch (state_) {
		case ClientTXState::NOT_STARTED: {
			assert(msgs.empty());

			// we started work with tx not before earliest_start_ts in tx spec.
			if (ts >= spec_.earliest_start_ts) {
				LOG_DEBUG << "[ClientTx::tick] ts=" << ts << " is greater/equal than earliest_start_ts=" << spec_.earliest_start_ts
					<< ", configure coordinator.";

				configure_coordinator(ts);
				progress_state(&state_);
			}
			break;
		}
		case ClientTXState::START_SENT: {
			process_replies_start_sent(msgs);

			assert(participants_.at(coordinator_actor_id_));
			if (participants_[coordinator_actor_id_]->is_open()) {
				configure_read_ts();
				progress_state(&state_);
			}
		}
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

void ClientTx::configure_read_ts() {
	read_ts_ = participants_[coordinator_actor_id_]->read_ts();
	LOG_DEBUG << "[ClientTx::tick][state=" << state_ << "]"
			  << " setup read_ts=" << read_ts_;
}

void ClientTx::process_replies_start_sent(const Messages& msgs) {
	// handled MSG_ROLLED_BACK_BY_SERVER
}

} // namespace sdb::tx::client