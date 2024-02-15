//
// Created by focus on 2/13/24.
//

#pragma once

#include <variant>

#include <tx/msg/msg_types.h>

namespace sdb::tx::msg {

struct MsgPayload final {
	std::variant<MsgStartPayload, MsgAckStartPayload> payload;

	template<class T>
	auto& get() {
		return std::get<T>(payload);
	}

	template<class T>
	const auto& get() const {
		return std::get<T>(payload);
	}
};

} // namespace sdb::tx::msg
