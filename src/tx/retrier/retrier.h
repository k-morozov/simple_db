//
// Created by focus on 2/15/24.
//

#pragma once

#include <tx/types.h>

namespace sdb::tx {

class Retrier {
public:
	using Handle = int64_t;

	~Retrier() = default;

	void send_once(Timestamp timestamp, msg::Message out);
	Handle schedule(Timestamp timestamp, msg::Message out);
	void get_scheduled_msgs(Timestamp ts,
							Messages* messages);

private:
	Messages schedule_msgs_;
};

} // namespace sdb::tx