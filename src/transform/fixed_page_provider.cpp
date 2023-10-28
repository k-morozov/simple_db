//
// Created by focus on 10/24/23.
//

#include "fixed_page_provider.h"

#include <transform/marshal.h>
#include <table/fixed_table_page.h>

namespace sdb {

class FixedPageProvider : public IPageProvider {
public:
	explicit FixedPageProvider(MarshalPtr marshal) :
		marshal_(std::move(marshal))
	{}

	~FixedPageProvider() override = default;

	tb::TablePagePtr transform(bp::PoolPagePtr page) override {
		return std::make_shared<tb::FixedTablePage>(std::move(page), marshal_);
	}

private:
	const MarshalPtr marshal_;
};

PageProviderPtr make_fixed_page_provider(SchemaPtr schema) {
	auto marshal = std::make_shared<Marshal>(std::move(schema));
	return std::make_shared<FixedPageProvider>(std::move(marshal));
}

} // namespace sdb