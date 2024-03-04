//
// Created by focus on 2/15/24.
//

#include "tx_spec.h"

namespace sdb::tx::client {

std::string CheckNone(const Timestamp ts) {
  if (ts == UNDEFINED_TS) return "None";
  return std::to_string(ts);
}

std::ostream& operator<<(std::ostream& stream, const TxSpec& spec) {
  stream << "[ClientTxSpec]"
         << "[earliest_start_ts=" << CheckNone(spec.earliest_start_ts) << "]"
         << "[earliest_commit_ts=" << CheckNone(spec.earliest_commit_ts) << "]";
  return stream;
}

}  // namespace sdb::tx::client
