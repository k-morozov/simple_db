//
// Created by focus on 2/15/24.
//

#include "client_tx_spec.h"

namespace sdb::tx::client {

std::ostream& operator<<(std::ostream& stream, const ClientTxSpec& spec) {
	stream << "[ClientTxSpec]"
		<< " earliest_start_ts=" << spec.earliest_start_ts
		<< " earliest_commit_ts=" << spec.earliest_commit_ts
		<< " todo another fields";
	return stream;
}

} // namespace sdb::tx::client
