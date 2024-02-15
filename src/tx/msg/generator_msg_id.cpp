//
// Created by focus on 2/15/24.
//

#include "generator_msg_id.h"

namespace sdb::tx::msg {

MsgID GeneratorMsgID::get_next_id() {
	static MsgID msgID = 0;
	return ++msgID;
}

} // namespace sdb::tx::msg