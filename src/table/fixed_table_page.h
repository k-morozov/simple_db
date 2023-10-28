//
// Created by focus on 10/24/23.
//

#pragma once

#include <table/table_page.h>
#include <transform/marshal.h>
#include <buffer_pool/pool_page.h>
#include <buffer_pool/fwd.h>

namespace sdb::tb {

class FixedTablePage : public ITablePage {
public:
	explicit FixedTablePage(bp::PoolPagePtr page, MarshalPtr marshal);

	~FixedTablePage() override = default;

	std::pair<bool, RowIndex> insert_row(const Row &row) override;

	Row get_row(RowIndex index) override;

private:
	const bp::PoolPagePtr page_;
	const MarshalPtr marshal_;

	std::pair<bool, RowIndex> find_free_slot() const;

	RowIndex max_row_index() const { return bp::PageSize / marshal_->fixed_row_space(); }

	uint8_t* data_by_index(RowIndex index) const;
};

} // namespace sdb::tb


