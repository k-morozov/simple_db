//
// Created by focus on 10/21/23.
//

#include "table.h"

namespace sdb::tb {

class Table : public ITable {
public:
	explicit Table(bp::BufferPoolPtr pool, bp::FilePtr file, PageProviderPtr provider) :
		pool_(std::move(pool)),
		file_(std::move(file)),
		provider_(std::move(provider)),
		current_page_index_(0)
	{}
	~Table() override = default;

	TablePagePtr get_page(bp::PageIndex index) override {
		auto pool_page = pool_->get_page(file_, index);
		return provider_->transform(pool_page);
	}

	RowID insert_row(const Row &row) override {
		for(;;current_page_index_++) {
			if (current_page_index_ == get_page_count()) {
				current_page_index_ = file_->alloc_page(); // @TODO
			}

			auto page = get_page(current_page_index_);

			if (auto [ok, row_index] = page->insert_row(row); ok) {
				return RowID{
					.page_index=current_page_index_,
					.row_index=row_index
				};
			}
		}
	}

	Row get_row(const RowID& id) override {
		auto page = get_page(id.page_index);
		return page->get_row(id.row_index);
	}


private:
	const bp::BufferPoolPtr pool_;
	const bp::FilePtr file_;
	const PageProviderPtr provider_;

	PageIndex current_page_index_;

	PageIndex get_page_count() const {
		return file_->get_page_count();
	}
};

TablePtr make_table(bp::BufferPoolPtr pool, bp::FilePtr file, PageProviderPtr provider) {
	return std::make_shared<Table>(std::move(pool), std::move(file), std::move(provider));
}

} // namespace sdb::tb
