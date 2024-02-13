//
// Created by focus on 2/13/24.
//

#pragma once

#include <variant>

#include <tx/msg/msg_types.h>

namespace sdb::tx {

struct MsgPayload final {
	std::variant<StartPayload, AckStartPayload> payload;

	template<class T>
	auto& get() {
		return std::get<T>(payload);
	}

	template<class T>
	const auto& get() const {
		return std::get<T>(payload);
	}
};

} // namespace sdb::tx
