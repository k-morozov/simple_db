//
// Created by focus on 10/24/23.
//

#pragma once

#include <cassert>
#include <memory>

#include <table/schema.h>
#include <table/row.h>

namespace sdb {

class Marshal;
using MarshalPtr = std::shared_ptr<Marshal>;

class Marshal final {
public:
	explicit Marshal(SchemaPtr schema);

	[[nodiscard]]
	size_t fixed_row_space() const {
		assert(fixed_row_space_ != 0);
		return fixed_row_space_;
	}

	static void serialize_row(uint8_t* data, const tb::Row& row);

	[[nodiscard]]
	tb::Row deserialize_row(uint8_t* data) const;

private:
	const SchemaPtr schema_;
	const size_t fixed_row_space_;
};

} // namespace sdb
