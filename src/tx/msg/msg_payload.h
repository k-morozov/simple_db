//
// Created by focus on 2/13/24.
//

#pragma once

#include <ostream>
#include <variant>

#include <tx/msg/msg_types.h>

namespace sdb::tx::msg {

struct MsgPayload final {
	std::variant<
		MsgStartPayload,
		MsgAckStartPayload,
		MsgPutPayload,
		MsgPutReplyPayload,
		MsgCommitPayload,
		MsgCommitAckPayload,
		MsgRolledBackByServerPayload
	> payload;

	template<class T>
	auto& get() {
		return std::get<T>(payload);
	}

	template<class T>
	const auto& get() const {
		return std::get<T>(payload);
	}

	template<class T>
	void set(T&& v) {
		payload = v;
	}
};

bool operator==(const MsgPayload& lhs, const MsgPayload& rhs);
std::ostream& operator<<(std::ostream& stream, const MsgPayload& payload);

} // namespace sdb::tx::msg
