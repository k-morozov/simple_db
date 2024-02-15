//
// Created by focus on 2/15/24.
//
#pragma once

#include <string>

#include <tx/types.h>
#include <tx/client/client_tx_spec.h>
#include <tx/retrier/retrier.h>

namespace sdb::tx::client {

enum class ClientTXState {
	// Initial state, before Start message is sent.
	NOT_STARTED,

	// Start message has been sent, and txid assigned.
	START_SENT,

	// Coordinator replied with StartAck and provided read_timestamp.
	OPEN,
};

std::string to_string(ClientTXState state);

class ClientTx final {
public:
	ClientTx(ActorID actor_id, const ClientTxSpec& spec, Retrier* retrier);

	void tick(Timestamp ts, const Messages& msgs, Messages* msg_out);

	ActorID get_actor_id() const noexcept { return actor_id_; }
	TxID get_tx_id() const noexcept { return txid_;}
private:
	const ActorID actor_id_;
	const ClientTxSpec spec_;
	Retrier* retrier_;

	ClientTXState state_{ClientTXState::NOT_STARTED};

	TxID txid_{UNDEFINED_TX_ID};

	Timestamp read_ts_{UNDEFINED_TS};
	Timestamp commit_ts_{UNDEFINED_TS};

};


} // namespace sdb::tx::client
