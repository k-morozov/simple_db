//
// Created by focus on 2/14/24.
//

#include "server_tx.h"

#include <cassert>

#include <common/log/log.h>
#include <tx/msg/message.h>
#include <tx/clock/clock.h>

namespace sdb::tx {

namespace {

} // namespace

std::string to_string(const ServerTXState state) {
	switch (state) {
		case ServerTXState::NOT_STARTED:
			return "NOT_STARTED";
		case ServerTXState::OPEN:
			return "OPEN";
	}
}

ServerTX::ServerTX(ActorID clientID, Storage *storage):
	clientID_(clientID),
	storage_(storage)
{
	LOG_DEBUG << "ServerTX with clientID=" << clientID << " created";
}

void ServerTX::tick(const Timestamp ts, Messages msgs, Messages* outgoing_msgs) {
	assert(outgoing_msgs);

	for(auto& msg : msgs) {
		LOG_DEBUG << "[ServerTX::tick] state=" << to_string(state_)
				  << " got msg=" << msg.type << " with msg_id=" << msg.msg_id;

		switch (state_) {
			case ServerTXState::NOT_STARTED:
				process_msg_not_started(ts, msg);
				break;
			case ServerTXState::OPEN:
				break;
		}
	}
}

void ServerTX::process_msg_not_started(const Timestamp ts, const msg::Message msg) {
	LOG_DEBUG << "[ServerTX::process_msg_not_started] called with ts=" << ts
		<< " msg=" << msg;

	switch (msg.type) {
		case msg::MessageType::MSG_START: {
			read_ts_ = ts;
			txid_ = msg.payload.get<msg::MsgStartPayload>().txid;

			const auto source_for_ack = msg.destination;
			const auto destination_for_ack = msg.source;

			const auto ack_msg = msg::CreateMsgStartAck(source_for_ack, destination_for_ack, txid_, read_ts_);
			LOG_DEBUG << "[ServerTX::process_msg_not_started] send ack: " << ack_msg;

			// @todo send ack

			LOG_INFO << "[ServerTX::process_msg_not_started] change state from " << to_string(state_)
				<< " to " << to_string(ServerTXState::OPEN);
			state_ = ServerTXState::OPEN;

			break;
		}

		default:
			report_unexpected_msg(msg);
	}
}

void ServerTX::report_unexpected_msg(const msg::Message msg) {
	LOG_ERROR << "[ServerTX][" << txid_ << "]" <<
		" state=" << to_string(state_)
		<< " gon unexpected msg=" << msg;
}


} // namespace sdb::tx