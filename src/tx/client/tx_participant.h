//
// Created by focus on 2/16/24.
//

#pragma once

#include <ostream>
#include <cassert>
#include <unordered_map>

#include <tx/types.h>
#include <tx/retrier/retrier.h>
#include <tx/client/requests_manager.h>

namespace sdb::tx::client {

enum class TxParticipantState {
	NOT_STARTED,
	START_MSG_SENT,
	TX_OPEN,
};

std::ostream& operator<<(std::ostream& stream, TxParticipantState state);

//struct RequestState final {
//	enum class Status {
//		REQUEST_NOT_STARTED,
//		REQUEST_START,
//		REQUEST_COMPLETED
//	};
//	Status status{Status::REQUEST_NOT_STARTED};
//	Key key;
//	Value value;
//};

class TxParticipant final {
public:
	friend std::ostream& operator<<(std::ostream& stream, TxParticipant self);

	// Create TxParticipant for a given client(client_tx_actor_id)
	// and server(coordinator_actor_id).
	TxParticipant(ActorID client_tx_actor_id,
				  ActorID coordinator_actor_id,
				  Retrier* retrier);

	// Ask the server for a read_ts.
	void schedule_start_msg(Timestamp ts);

	bool is_open() const noexcept { return state_ == TxParticipantState::TX_OPEN; }
	Timestamp read_ts() const noexcept { assert(read_ts_ != UNDEFINED_TS); return read_ts_; }

	TxID txid() const;

	void process_incoming(Timestamp ts, const Messages& msgs);

	void maybe_issue_requests(Timestamp ts);

	void issue_put(Key key, Value value);

	size_t completed_puts() const { return req_manager_.completed_puts(); }

	void export_results(std::vector<std::pair<Key, Value>>* puts) const;

private:
	const ActorID client_tx_actor_id_;
	const ActorID coordinator_actor_id_;
	Retrier* retrier_;
	RequestsManager req_manager_{};

	TxID txid_{UNDEFINED_TX_ID};
	Timestamp read_ts_{UNDEFINED_TS};

	TxParticipantState state_{TxParticipantState::NOT_STARTED};

private:
	/*
	 * We can setup read_ts from msg and change state to OPEN.
	 * */
	void process_replies_start_sent(const Messages& msgs);

	void process_replies_open(const Messages& msgs);
};



} // namespace sdb::tx::client