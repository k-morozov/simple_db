//
// Created by focus on 2/15/24.
//

#include "msg_payload.h"

namespace sdb::tx::msg {

namespace {

template<class ...Args>
std::ostream& operator<<(std::ostream& stream, const std::variant<Args...>& args) {
	std::visit([&stream](auto& value) {
		stream << value;
	}, args);
	return stream;
}

} // namespace

bool operator==(const MsgPayload& lhs, const MsgPayload& rhs) {
	if (lhs.payload.index() != rhs.payload.index()) {
		return false;
	}
	return lhs.payload == rhs.payload;
}

std::ostream& operator<<(std::ostream& stream, const MsgPayload& payload) {
	stream << payload.payload;
	return stream;
}


} // namespace sdb::tx::msg
