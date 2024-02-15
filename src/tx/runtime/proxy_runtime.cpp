//
// Created by focus on 2/12/24.
//

#include "proxy_runtime.h"

#include <cassert>
#include <common/log/log.h>

namespace sdb::tx {

ProxyRuntime::ProxyRuntime(RuntimePtr runtime, const ActorID source_actor_id):
	runtime(std::move(runtime)),
	source_actor_id(source_actor_id)
{
	LOG_DEBUG << "Create runtime proxy for sender " << source_actor_id;
}

void ProxyRuntime::send(msg::Message msg) {
	LOG_DEBUG << "[ProxyRuntime::send] set source=" << source_actor_id
		<< " for " << msg;
	msg.source = source_actor_id;

	LOG_DEBUG << "[ProxyRuntime::send] msg"
			  << " with id=" << msg.msg_id
			  << " to runtime";

	assert(runtime);
	runtime->send(std::move(msg));
}
} // namespace sdb::tx