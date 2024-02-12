//
// Created by focus on 2/10/24.
//

#include "clock.h"

namespace sdb::tx {


Timestamp Clock::current() const {
	return tm_;
}

Timestamp Clock::next() {
	return ++tm_;
}

} // namespace sdb::tx