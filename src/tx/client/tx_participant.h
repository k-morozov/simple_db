//
// Created by focus on 2/16/24.
//

#pragma once

#include <ostream>
#include <cassert>

#include <tx/types.h>
#include <tx/retrier/retrier.h>

namespace sdb::tx::client {

enum class TxParticipantState {
	NOT_STARTED,
	START_SENT,
	OPEN,
};

std::ostream& operator<<(std::ostream& stream, TxParticipantState state);

class TxParticipant final {
public:
	friend std::ostream& operator<<(std::ostream& stream, TxParticipant self);

	// Create TxParticipant for a given client(client_tx_actor_id)
	// and server(coordinator_actor_id).
	TxParticipant(ActorID client_tx_actor_id,
				  ActorID coordinator_actor_id,
				  Retrier* retrier);

	// Ask the server for a read_ts.
	void start(Timestamp ts);

	bool is_open() const noexcept { return state_ == TxParticipantState::OPEN; }
	Timestamp read_ts() const noexcept { assert(read_ts_ != UNDEFINED_TS); return read_ts_; }

	TxID txid() const;

	void process_incoming(Timestamp ts, const Messages& msgs);

private:
	const ActorID client_tx_actor_id_;
	const ActorID coordinator_actor_id_;
	Retrier* retrier_;

	TxID txid_{UNDEFINED_TX_ID};
	Timestamp read_ts_{UNDEFINED_TS};

	TxParticipantState state_{TxParticipantState::NOT_STARTED};

	/*
	 * We can setup read_ts from msg and change state to OPEN.
	 * */
	void process_replies_start_sent(const Messages& msgs);
};



} // namespace sdb::tx::client