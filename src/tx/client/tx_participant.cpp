//
// Created by focus on 2/16/24.
//

#include "tx_participant.h"

#include <cassert>

#include <common/log/log.h>
#include <tx/msg/message.h>

namespace sdb::tx::client {

std::ostream& operator<<(std::ostream& stream, const TxParticipant self) {
	stream << "[TxParticipant]"
		<< "[client=" << self.client_tx_actor_id_ << "]"
		<< "[server=" << self.coordinator_actor_id_ << "]"
		<< "[txid=" << self.txid_ << "]"
		<< "[state=" << self.state_ << "]";

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const TxParticipantState state) {
	switch (state) {
		case TxParticipantState::NOT_STARTED:
			stream << "NOT_STARTED";
			break;
		case TxParticipantState::START_SENT:
			stream << "START_SENT";
			break;
		case TxParticipantState::OPEN:
			stream << "OPEN";
			break;
	}
	return stream;
}

TxParticipant::TxParticipant(const ActorID client_tx_actor_id,
							 const ActorID coordinator_actor_id,
							 Retrier *retrier):
	client_tx_actor_id_(client_tx_actor_id),
	coordinator_actor_id_(coordinator_actor_id),
	retrier_(retrier)
{
	LOG_SELF_DEBUG << "created.";
}

void TxParticipant::start(const Timestamp ts) {
	auto create_msg = msg::CreateMsgStart(client_tx_actor_id_, coordinator_actor_id_);
	txid_ = msg::get_txid_from_msg_payload(create_msg);

	LOG_DEBUG << "[TxParticipant::start] create and send " << create_msg;

	assert(txid_ != UNDEFINED_TX_ID);

	retrier_->schedule(ts, create_msg);

	LOG_DEBUG << "[TxParticipant::start] change state to " << TxParticipantState::START_SENT;

	state_ = TxParticipantState::START_SENT;
}

TxID TxParticipant::txid() const {
	assert(txid_ != UNDEFINED_TX_ID);
	return txid_;
}

void TxParticipant::process_incoming(const Timestamp ts, const Messages &msgs) {
	LOG_DEBUG << "[TxParticipant::process_incoming][ts=" <<
		ts << "][state=" << state_ << "] called";

	switch (state_) {
		case TxParticipantState::NOT_STARTED:
			throw std::invalid_argument("process_incoming not working with NOT_STARTED state");
		case TxParticipantState::START_SENT:
			process_replies_start_sent(msgs);
			break;
		case TxParticipantState::OPEN:
			process_replies_open(msgs);
	}
}

void TxParticipant::maybe_issue_requests(const Timestamp ts) {
	if(state_ != TxParticipantState::OPEN) {
		LOG_DEBUG << "[TxParticipant::maybe_issue_requests] available only in OPEN state";
		return;
	}

	for(; next_put_ < put_status_.size(); next_put_++) {
		put_status_[next_put_].status = RequestState::Status::REQUEST_START;

		auto msg = msg::CreateMsgPut(client_tx_actor_id_, coordinator_actor_id_, txid_,
									 put_status_[next_put_].key, put_status_[next_put_].value);
		retrier_->schedule(ts, msg);
		put_request_[msg.msg_id] = next_put_;
	}
}

void TxParticipant::process_replies_start_sent(const Messages &msgs) {
	Timestamp ts{UNDEFINED_TS};

	for(const auto& msg : msgs) {
		switch (msg.type) {
			case msg::MessageType::MSG_START_ACK:
				ts = msg.payload.get<msg::MsgAckStartPayload>().read_ts;
				break;
			default:
				throw std::logic_error("process_replies_start_sent work only with MSG_START_ACK");
		}
	}

	assert(read_ts_ == UNDEFINED_TS);
	assert(ts != UNDEFINED_TS);

	read_ts_ = ts;

	constexpr auto next_state = TxParticipantState::OPEN;

	LOG_DEBUG << "[TxParticipant::process_replies_start_sent]"
		<< " setup read_ts=" << read_ts_
		<< " and change state from " << state_
		<< " to " << next_state;

	state_ = next_state;
}

void TxParticipant::process_replies_open(const Messages& msgs) {
	for(const auto& msg : msgs) {
		LOG_DEBUG << "[TxParticipant::process_replies_open]" << msg;

		switch (msg.type) {
			case msg::MessageType::MSG_PUT_REPLY: {
				const auto original_msg_id = msg.payload.get<msg::MsgPutReplyPayload>().msg_id;
				if (put_request_.contains(original_msg_id)) {
					const auto old_next_put = put_request_[original_msg_id];
					if (put_status_[old_next_put].status == RequestState::Status::REQUEST_START) {
						put_status_[old_next_put].status = RequestState::Status::REQUEST_COMPLETED;
						completed_puts_++;

						LOG_DEBUG << "[TxParticipant::process_replies_open]"
							<< " old_next_put=" << old_next_put << " set status REQUEST_COMPLETED"
							<< ", completed_puts=" << completed_puts_;
					}
				} else {
					// log error
				}
				break;
			}
			default:
				throw std::logic_error("process_replies_open work only with MSG_PUT_REPLY");
		}
	}
}

void TxParticipant::issue_put(Key key, Value value) {
	put_status_.push_back(RequestState{
		.status=RequestState::Status::REQUEST_NOT_STARTED,
		.key=key,
		.value=value
	});
}

} // namespace sdb::tx::client