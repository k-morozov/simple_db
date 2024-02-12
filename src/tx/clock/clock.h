//
// Created by focus on 2/10/24.
//

#pragma once

#include <tx/types.h>

namespace sdb::tx {

class Clock final {
public:
	Timestamp current() const;
	Timestamp next();

private:
	Timestamp tm_{0};
};

} // namespace sdb::tx