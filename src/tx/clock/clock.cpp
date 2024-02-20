//
// Created by focus on 2/10/24.
//

#include "clock.h"

#include <common/log/log.h>

namespace sdb::tx {


Timestamp Clock::current() const {
	return tm_;
}

Timestamp Clock::next() {
	LOG_DEBUG << "[Clock::next] up ts=" << tm_+1;
	return ++tm_;
}

} // namespace sdb::tx