//
// Created by focus on 10/24/23.
//

#pragma once

#include <cstdlib>
#include <cstdint>
#include <functional>
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

inline
bool operator==(const NullAttribute& lhs, const NullAttribute& rhs) {
	return true;
}

using PageIndex = size_t;
using RowIndex = size_t;

using Row = std::vector<std::variant<NullAttribute, bool, uint64_t>>;

struct RowID final {
	PageIndex page_index;
	RowIndex row_index;

	bool operator==(const RowID&) const = default;
};

} // namespace sdb::tb

namespace std {

template<>
struct hash<sdb::tb::RowID> {
	std::size_t operator()(const sdb::tb::RowID &v) const noexcept {
		return hash<sdb::tb::PageIndex>()(v.page_index) ^ hash<sdb::tb::RowIndex>()(v.row_index);
	}
};

}