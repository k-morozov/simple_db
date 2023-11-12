//
// Created by focus on 10/24/23.
//

#include "fixed_table_page.h"

#include <cassert>

namespace sdb::tb {

FixedTablePage::FixedTablePage(bp::PoolPagePtr page, MarshalPtr marshal) :
		page_(std::move(page)),
		marshal_(std::move(marshal))
{}

std::pair<bool, RowIndex> FixedTablePage::insert_row(const Row &row) {
	if (auto [ok, row_index] = find_free_slot(); ok) {
		uint8_t* row_header = data_by_index(row_index);
		*reinterpret_cast<bool*>(row_header) = true;
		uint8_t* row_data = row_header + 1;
		marshal_->serialize_row(row_data, row);

		const auto actual_row = marshal_->deserialize_row(row_data);
		assert(actual_row == row);

		return {true, row_index};
	}
	return {false, 0};
}

Row FixedTablePage::get_row(const RowIndex index) {
	uint8_t* row_header = data_by_index(index);
	if (static_cast<bool>(row_header[0])) {
		uint8_t* row_data = row_header + 1;
		return marshal_->deserialize_row(row_data);
	}
	return {};
}

std::pair<bool, RowIndex> FixedTablePage::find_free_slot() const {
	for(RowIndex index=0; index<max_row_index(); index++) {
		const uint8_t* const index_data = data_by_index(index);
		if (!static_cast<bool>(index_data[0])) {
			return {true, index};
		}
	}
	return {false, 0};
}

uint8_t* FixedTablePage::data_by_index(RowIndex index) const {
	// @TODO think about +1
	return page_->get_data() + index * (1 + marshal_->fixed_row_space());
}


} // namespace sdb::tb