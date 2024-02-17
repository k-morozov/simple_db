//
// Created by focus on 2/15/24.
//
#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <tx/types.h>
#include <tx/client/tx_spec.h>
#include <tx/client/tx_participant.h>
#include <tx/retrier/retrier.h>
#include <tx/discovery/discovery.h>

namespace sdb::tx::client {

enum class ClientTXState {
	// Initial state, before Start message is sent.
	NOT_STARTED,

	// Start message has been sent, and txid assigned.
	START_SENT,

	// Coordinator replied with StartAck and provided read_timestamp.
	OPEN,
};

std::ostream& operator<<(std::ostream& stream, const ClientTXState& state);
void progress_state(ClientTXState* state);

class ClientTx final {
public:
	friend std::ostream& operator<<(std::ostream& stream, const ClientTx& self);

	ClientTx(ActorID actor_id, const TxSpec& spec, const Discovery* discovery, Retrier* retrier);

	void tick(Timestamp ts, const Messages& msgs, Messages* msg_out);

	ActorID get_actor_id() const noexcept { return actor_id_; }
	TxID get_tx_id() const noexcept { return txid_;}
private:
	const ActorID actor_id_;
	const TxSpec spec_;
	const Discovery* discovery_;
	Retrier* retrier_;

	ClientTXState state_{ClientTXState::NOT_STARTED};

	ActorID coordinator_actor_id_{UNDEFINED_ACTOR_ID};

	std::unordered_map<ActorID, std::unique_ptr<TxParticipant>> participants_;

	TxID txid_{UNDEFINED_TX_ID};

	Timestamp read_ts_{UNDEFINED_TS};
	Timestamp commit_ts_{UNDEFINED_TS};

private:
	void configure_coordinator(Timestamp ts);
	void configure_read_ts();

	void process_replies_start_sent(const Messages& msgs);
};


} // namespace sdb::tx::client
