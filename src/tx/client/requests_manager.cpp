//
// Created by focus on 2/22/24.
//

#include "requests_manager.h"

#include <common/log/log.h>

namespace sdb::tx::client {

void RequestsManager::complete_request(const MsgID original_msg_id) {
	if (table_scheduled_puts_.contains(original_msg_id)) {
		const auto old_next_put = table_scheduled_puts_[original_msg_id];
		if (puts_table_status_[old_next_put].status == RequestState::Status::REQUEST_START) {
			puts_table_status_[old_next_put].status = RequestState::Status::REQUEST_COMPLETED;
			completed_puts_++;

			LOG_DEBUG << "[RequestsManager::complete_request]"
					  << " old_next_put=" << old_next_put << " set status REQUEST_COMPLETED"
					  << ", completed_puts=" << completed_puts_;
		}
	} else {
		LOG_ERROR << "[RequestsManager::complete_request] not found original_msg_id in put_request";
	}
}


void RequestsManager::add_put(Key key, Value value) {
	LOG_DEBUG << "[RequestsManager::add_put] add request to puts_table_status with key="
		<< key << " and value=" << value;
	puts_table_status_.push_back(RequestState{
			.status=RequestState::Status::REQUEST_NOT_STARTED,
			.key=key,
			.value=value
	});
}

void RequestsManager::prepare_msg(const ActorID client_tx_actor_id,
								  const ActorID coordinator_actor_id,
								  const TxID txid,
								  std::function<void(msg::Message&&)> scheduler) {
	for(; next_put_ < puts_table_status_.size(); next_put_++) {
		puts_table_status_[next_put_].status = RequestState::Status::REQUEST_START;

		auto msg = msg::CreateMsgPut(client_tx_actor_id,
									 coordinator_actor_id, txid,
									 puts_table_status_[next_put_].key,
									 puts_table_status_[next_put_].value);

		scheduler(std::move(msg));

		table_scheduled_puts_[msg.msg_id] = next_put_;
	}
}

void RequestsManager::export_results(std::vector<std::pair<Key, Value>> *puts) const {
	for(const auto& status:puts_table_status_) {
		if (status.status != RequestState::Status::REQUEST_COMPLETED) {
			continue;
		}
		puts->push_back(std::make_pair(status.key, status.value));
	}
}

} // namespace sdb::tx::client