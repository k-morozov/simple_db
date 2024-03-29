//
// Created by focus on 2/15/24.
//

#pragma once

#include <tx/types.h>

#include <ostream>
#include <vector>

namespace sdb::tx::client {

/*
 *
 * TxSpec describes what should be done in a given
 * transaction. It lists all Get and Put requests that the client
 * will perform in the transaction.
 * It also specifies whether the transaction is committed or rolled
 * back in the end.
 *
 * */

struct ClientTxGet {
  Timestamp earliest_ts;
  Key key;
};

struct ClientTxPut {
  Timestamp earliest_ts{UNDEFINED_TS};
  Key key;
  Value value;
};

struct TxSpec final {
  // Earliest timestamp to start the transaction.
  Timestamp earliest_start_ts{UNDEFINED_TS};

  // Earliest timestamp to commit the transaction.
  Timestamp earliest_commit_ts{UNDEFINED_TS};

  std::vector<ClientTxGet> gets;
  std::vector<ClientTxPut> puts;

  enum class Action {
    UNDEFINED,
    COMMIT,
    ROLLBACK,
  };

  Action action{Action::UNDEFINED};
};

std::ostream& operator<<(std::ostream& stream, const TxSpec& spec);

}  // namespace sdb::tx::client
