//
// Created by focus on 2/16/24.
//

#include "discovery.h"

#include <stdexcept>

#include <common/log/log.h>

namespace sdb::tx {

Discovery::Discovery(const ActorID actor_id):
	actor_ids_({actor_id})
{
	LOG_DEBUG << "Discovery created for single node system.";
}

Discovery::Discovery(const std::vector<ActorID> &actor_ids,
					 const KeyIntervals& intervals) :
	actor_ids_(actor_ids),
	intervals_(intervals)
{
	LOG_DEBUG << "Discovery created for multi-node system.";
}

ActorID Discovery::get_by_key(Key k) const {
	if (actor_ids_.size() == 1) {
		return actor_ids_.front();
	}
	throw std::invalid_argument("Discovery for multi node haven't supported yet.");
}

ActorID Discovery::get_random() const {
	if (actor_ids_.size() == 1) {
		return actor_ids_.front();
	}
	throw std::invalid_argument("Discovery for multi node haven't supported yet.");
}
} // namespace sdb::tx