//
// Created by focus on 2/16/24.
//

#pragma once

#include <tx/types.h>

namespace sdb::tx {

// Discovery finds server node by key.
// Each server node works with permanent KeyInterval.
class Discovery final {
public:
	// Single-node system
	explicit Discovery(ActorID);

	Discovery(const std::vector<ActorID>& actor_ids, const KeyIntervals& intervals);

	ActorID get_by_key(Key k) const;
	ActorID get_random() const;

private:
	const std::vector<ActorID> actor_ids_;
	const KeyIntervals intervals_;
};


} // namespace sdb::tx