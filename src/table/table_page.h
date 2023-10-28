//
// Created by focus on 10/24/23.
//

#pragma once

#include <memory>
#include <utility>

#include <table/row.h>

namespace sdb::tb {

class ITablePage;
using TablePagePtr = std::shared_ptr<ITablePage>;

class ITablePage {
public:
	virtual ~ITablePage() = default;

	virtual std::pair<bool, RowIndex> insert_row(const Row& row) = 0;
	virtual Row get_row(RowIndex index) = 0;
};

} // namespace sdb::tb