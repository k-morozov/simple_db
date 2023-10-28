//
// Created by focus on 10/21/23.
//

#pragma once

#include <memory>

#include <buffer_pool/buffer_pool.h>
#include <table/table_page.h>
#include <transform/page_provider.h>

namespace sdb::tb {

class ITable;
using TablePtr = std::shared_ptr<ITable>;

TablePtr make_table(bp::BufferPoolPtr pool, bp::FilePtr file, PageProviderPtr provider);

class ITable {
public:
	virtual ~ITable() = default;

	virtual TablePagePtr get_page(bp::PageIndex index) = 0;
	virtual RowID insert_row(const Row& row) = 0;
	virtual Row get_row(const RowID& id) = 0;
};

} // namespace sdb::tb
