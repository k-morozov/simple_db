//
// Created by focus on 2/12/24.
//

#pragma once

#include <tx/types.h>
#include <tx/runtime/runtime.h>

namespace sdb::tx {

class ProxyRuntime final {
public:
	ProxyRuntime(RuntimePtr runtime, ActorID source_actor_id);

	void send(Message msg);

private:
	const RuntimePtr runtime;
	const ActorID source_actor_id;
};

} // namespace sdb::tx
