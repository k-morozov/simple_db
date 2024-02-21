//
// Created by focus on 2/14/24.
//

#include "server_tx.h"

#include <cassert>

#include <common/log/log.h>
#include <tx/msg/message.h>
#include <tx/clock/clock.h>
#include <tx/retrier/retrier.h>
#include <tx/storage/storage.h>

namespace sdb::tx::server {

namespace {

} // namespace

std::string to_string(const ServerTXState state) {
	switch (state) {
		case ServerTXState::NOT_STARTED:
			return "NOT_STARTED";
		case ServerTXState::OPEN:
			return "OPEN";
		case ServerTXState::COMMITTED:
			return "COMMITTED";
		case ServerTXState::ROLLED_BACK_BY_SERVER:
			return "ROLLED_BACK_BY_SERVER";
	}
}

ServerTX::ServerTX(ActorID clientID, Storage *storage, Retrier* retrier):
	clientID_(clientID),
	storage_(storage),
	retrier_(retrier)
{
	assert(storage_);
	assert(retrier_);

	LOG_DEBUG << "ServerTX with clientID=" << clientID << " created";
}

void ServerTX::tick(const Timestamp ts, Messages msgs, Messages* scheduled_msgs) {
	assert(scheduled_msgs);

	for(auto& msg : msgs) {
		LOG_DEBUG << "[ServerTX::tick] state=" << to_string(state_)
				  << " got msg=" << msg.type << " with msg_id=" << msg.msg_id;

		switch (state_) {
			case ServerTXState::NOT_STARTED:
				process_msg_not_started(ts, msg);
				break;
			case ServerTXState::OPEN:
				process_msg_open(ts, msg);
				break;
			case ServerTXState::COMMITTED:
			case ServerTXState::ROLLED_BACK_BY_SERVER:
				throw std::runtime_error("server state has broken switch");
		}
	}

	retrier_->get_scheduled_msgs(ts, scheduled_msgs);
}

void ServerTX::process_msg_not_started(const Timestamp ts, const msg::Message msg) {
	LOG_DEBUG << "[ServerTX::process_msg_not_started][read_ts=" << read_ts_
		<< "] called with ts=" << ts << " msg=" << msg;

	switch (msg.type) {
		case msg::MessageType::MSG_START: {
			assert(read_ts_ == UNDEFINED_TS);
			read_ts_ = ts;
			txid_ = msg.payload.get<msg::MsgStartPayload>().txid;

			LOG_DEBUG << "[ServerTX::process_msg_not_started] setup txid=" << txid_;

			const auto source_for_ack = msg.destination;
			const auto destination_for_ack = msg.source;

			const auto ack_msg = msg::CreateMsgStartAck(source_for_ack, destination_for_ack, txid_, read_ts_);
			LOG_DEBUG << "[ServerTX::process_msg_not_started] schedule ack: " << ack_msg;

			[[maybe_unused]]
			auto handle = retrier_->schedule(read_ts_, ack_msg);

			LOG_INFO << "[ServerTX::process_msg_not_started] change state from " << to_string(state_)
				<< " to " << to_string(ServerTXState::OPEN);
			state_ = ServerTXState::OPEN;

			break;
		}

		default:
			report_unexpected_msg(msg);
	}
}

void ServerTX::process_msg_open(Timestamp ts, msg::Message msg) {
	LOG_DEBUG << "[ServerTX::process_msg_open] called with ts=" << ts
			  << " msg=" << msg;

	switch (msg.type) {
		case msg::MessageType::MSG_PUT: {
			const auto key = msg.payload.get<msg::MsgPutPayload>().key;
			const auto value = msg.payload.get<msg::MsgPutPayload>().value;

			puts_.push_back({key, value});

			auto msg_put_reply = msg::CreateMsgPutReply(msg.destination, msg.source, txid_, msg.msg_id);

			LOG_DEBUG << "[ServerTX::process_msg_open] send put reply: " << msg_put_reply;

			retrier_->send_once(ts, msg_put_reply);

			break;
		}
		case msg::MessageType::MSG_COMMIT: {
			commit_ts_ = ts;
			LOG_DEBUG << "[ServerTX::process_msg_open][read_ts=" << read_ts_ << "] setup commit_ts=" << commit_ts_;

			bool has_conflict = false;
			TxID conflict_txid{UNDEFINED_TX_ID};

			for (const auto& put:puts_) {
				const auto* p = storage_->get_last(put.key);
				if (!p)
					continue;

				const Timestamp old_key_ts = p->first;
				LOG_DEBUG << "[ServerTX::process_msg_open] in storage key=" << put.key
					<< " has last: value=" << p->second << " and ts=" << old_key_ts;

				if (old_key_ts > read_ts_) {
					has_conflict = true;
					conflict_txid = p->second.txid;
					break;
				}
			}

			if (has_conflict) {
				state_ = ServerTXState::ROLLED_BACK_BY_SERVER;
				const auto msg_rolled_back = msg::CreateMsgRolledBackByServer(msg.destination, msg.source, txid_, conflict_txid);
				LOG_DEBUG << "[ServerTX::process_msg_open] send " << msg_rolled_back;
				retrier_->schedule(ts, msg_rolled_back);
				break;
			}

			state_ = ServerTXState::COMMITTED;

			for(const auto& put : puts_) {
				storage_->apply(txid_, commit_ts_, put.key, put.value);
			}

			const auto msg_commit_ack = msg::CreateMsgCommitAck(msg.destination, msg.source, txid_, commit_ts_);
			LOG_DEBUG << "[ServerTX::process_msg_open] send " << msg_commit_ack;
			retrier_->schedule(ts, msg_commit_ack);
			break;
		}
		default:
			break;
	}
}

void ServerTX::report_unexpected_msg(const msg::Message msg) {
	LOG_ERROR << "[ServerTX][" << txid_ << "]" <<
		" state=" << to_string(state_)
		<< " gon unexpected msg=" << msg;
}


} // namespace sdb::tx::server