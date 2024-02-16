//
// Created by focus on 2/16/24.
//

#pragma once

#include <ostream>

#include <tx/types.h>
#include <tx/retrier/retrier.h>

namespace sdb::tx::client {

enum class TxParticipantState {
	NOT_STARTED,
	START_SENT,
	OPEN,
};

std::ostream& operator<<(std::ostream& stream, TxParticipantState state);

class TxParticipant {
public:
	friend std::ostream& operator<<(std::ostream& stream, TxParticipant self);

	// Create TxParticipant for a given client(client_tx_actor_id)
	// and server(coordinator_actor_id).
	TxParticipant(ActorID client_tx_actor_id,
				  ActorID coordinator_actor_id,
				  Retrier* retrier);

	// Ask the server for a read_ts.
	void start(Timestamp ts);

	TxID txid() const;

private:
	const ActorID client_tx_actor_id_;
	const ActorID coordinator_actor_id_;
	Retrier* retrier_;

	TxID txid_;

	TxParticipantState state_{TxParticipantState::NOT_STARTED};
};



} // namespace sdb::tx::client