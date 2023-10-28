//
// Created by focus on 10/24/23.
//

#pragma once

#include <cstdlib>
#include <cstdint>
#include <ostream>
#include <variant>
#include <vector>

namespace sdb::tb {

struct NullAttribute {};

inline
std::ostream& operator<<(std::ostream& stream, const NullAttribute& arg) {
	stream << "null";
	return stream;
}

using PageIndex = size_t;
using RowIndex = size_t;

using Row = std::vector<std::variant<NullAttribute, bool, uint64_t>>;

struct RowID final {
	PageIndex page_index;
	RowIndex row_index;
};

} // namespace sdb::tb