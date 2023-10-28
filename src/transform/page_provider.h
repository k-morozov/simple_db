//
// Created by focus on 10/24/23.
//

#pragma once

#include <memory>

#include <buffer_pool/fwd.h>
#include <table/table_page.h>

namespace sdb {

class IPageProvider;
using PageProviderPtr = std::shared_ptr<IPageProvider>;

class IPageProvider {
public:
	virtual ~IPageProvider() = default;

	virtual tb::TablePagePtr transform(bp::PoolPagePtr page) = 0;
};

} // namespace sdb
