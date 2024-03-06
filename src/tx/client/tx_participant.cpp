//
// Created by focus on 2/16/24.
//

#include "tx_participant.h"

#include <common/log/log.h>
#include <tx/msg/message.h>

#include <cassert>

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
    case TxParticipantState::START_MSG_SENT:
      stream << "START_MSG_SENT";
      break;
    case TxParticipantState::TX_OPEN:
      stream << "TX_OPEN";
      break;
  }
  return stream;
}

TxParticipant::TxParticipant(const ActorID client_tx_actor_id,
                             const ActorID coordinator_actor_id,
                             Retrier* retrier)
    : client_tx_actor_id_(client_tx_actor_id),
      coordinator_actor_id_(coordinator_actor_id),
      retrier_(retrier) {
  LOG_SELF_DEBUG << "created.";
}

void TxParticipant::schedule_start_msg(Timestamp ts) {
  auto create_msg =
      msg::CreateMsgStart(client_tx_actor_id_, coordinator_actor_id_);
  txid_ = msg::get_txid_from_msg_payload(create_msg);

  LOG_DEBUG << "[TxParticipant::schedule_start_msg] create and schedule "
            << create_msg;

  assert(txid_ != UNDEFINED_TX_ID);

  retrier_->schedule(ts, create_msg);

  LOG_DEBUG << "[TxParticipant::schedule_start_msg] change state to "
            << TxParticipantState::START_MSG_SENT;

  state_ = TxParticipantState::START_MSG_SENT;
}

TxID TxParticipant::txid() const {
  assert(txid_ != UNDEFINED_TX_ID);
  return txid_;
}

void TxParticipant::process_incoming(const Timestamp ts, const Messages& msgs) {
  LOG_DEBUG << "[TxParticipant::process_incoming][ts=" << ts
            << "][state=" << state_ << "] called";

  switch (state_) {
    case TxParticipantState::NOT_STARTED:
      throw std::invalid_argument(
          "process_incoming not working with NOT_STARTED state");
    case TxParticipantState::START_MSG_SENT:
      process_replies_start_sent(msgs);
      break;
    case TxParticipantState::TX_OPEN:
      process_replies_open(msgs);
  }
}

void TxParticipant::schedule_requests(const Timestamp ts) {
  if (state_ != TxParticipantState::TX_OPEN) {
    LOG_DEBUG << "[TxParticipant::schedule_requests][state=" << state_
              << "] available only in OPEN state";
    return;
  }

  req_manager_.prepare_msg(
      client_tx_actor_id_, coordinator_actor_id_, txid_,
      [this, ts](msg::Message&& msg) {
        LOG_DEBUG << "[TxParticipant::schedule_requests][state=" << state_
                  << "] scheduled ts=" << ts << ", " << msg;
        retrier_->schedule(ts, std::move(msg));
      });
}

void TxParticipant::process_replies_start_sent(const Messages& msgs) {
  if (msgs.empty()) {
    LOG_DEBUG
        << "[TxParticipant::process_replies_start_sent] msgs is empty. skip.";
    return;
  }

  Timestamp ts{UNDEFINED_TS};

  for (const auto& msg : msgs) {
    switch (msg.type) {
      case msg::MessageType::MSG_START_ACK:
        ts = msg.payload.get<msg::MsgAckStartPayload>().read_ts;
        assert(ts != UNDEFINED_TS);
        break;
      default:
        throw std::logic_error(
            "process_replies_start_sent work only with MSG_START_ACK");
    }
  }

  assert(read_ts_ == UNDEFINED_TS);
  assert(ts != UNDEFINED_TS);

  read_ts_ = ts;

  constexpr auto next_state = TxParticipantState::TX_OPEN;

  LOG_DEBUG << "[TxParticipant::process_replies_start_sent]"
            << " setup read_ts=" << read_ts_ << " and change state from "
            << state_ << " to " << next_state;

  state_ = next_state;
}

void TxParticipant::process_replies_open(const Messages& msgs) {
  for (const auto& msg : msgs) {
    LOG_DEBUG << "[TxParticipant::process_replies_open]" << msg;

    switch (msg.type) {
      case msg::MessageType::MSG_PUT_REPLY: {
        const auto original_msg_id =
            msg.payload.get<msg::MsgPutReplyPayload>().msg_id;
        req_manager_.complete_request(original_msg_id);
        break;
      }
      default:
        LOG_DEBUG << "[TxParticipant::process_replies_open] fix a bug: " << msg;
    }
  }
}

void TxParticipant::add_put(Key key, Value value) {
  req_manager_.add_put(key, value);
}

void TxParticipant::export_results(
    std::vector<std::pair<Key, Value>>* puts) const {
  req_manager_.export_results(puts);
}

}  // namespace sdb::tx::client