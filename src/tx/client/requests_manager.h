//
// Created by focus on 2/22/24.
//

#pragma once

#include <tx/msg/message.h>
#include <tx/types.h>

#include <cstdlib>
#include <functional>
#include <unordered_map>

namespace sdb::tx::client {

struct RequestState final {
  enum class Status { REQUEST_NOT_STARTED, REQUEST_START, REQUEST_COMPLETED };

  Status status{Status::REQUEST_NOT_STARTED};
  Key key{};
  Value value{};
};

class RequestsManager final {
 public:
  void add_put(Key key, Value value);

  void prepare_msg(ActorID client_tx_actor_id, ActorID coordinator_actor_id,
                   TxID txid, std::function<void(msg::Message&&)> scheduler);

  void complete_request(MsgID original_msg_id);

  size_t completed_puts() const { return completed_puts_; }

  void export_results(std::vector<std::pair<Key, Value>>* puts) const;

 private:
  std::vector<RequestState> puts_table_status_;

  size_t next_put_{0};
  size_t completed_puts_{0};

  std::unordered_map<MsgID, size_t> table_scheduled_puts_;
};

}  // namespace sdb::tx::client
