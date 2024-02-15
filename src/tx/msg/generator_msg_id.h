//
// Created by focus on 2/15/24.
//

#pragma once

#include <tx/types.h>

namespace sdb::tx::msg {

class GeneratorMsgID final {
public:
	static MsgID get_next_id();

};

} // namespace sdb::tx::msg