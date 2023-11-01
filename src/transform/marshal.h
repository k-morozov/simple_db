//
// Created by focus on 10/24/23.
//

#pragma once

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
	size_t fixed_row_space() const { return fixed_row_space_; }

	void serialize_row(uint8_t* data, const tb::Row& row) const;

	[[nodiscard]]
	tb::Row deserialize_row(uint8_t* data) const;

private:
	const SchemaPtr schema_;
	const size_t fixed_row_space_;
};

} // namespace sdb
