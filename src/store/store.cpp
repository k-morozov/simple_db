//
// Created by focus on 10/18/23.
//

#include "store.h"

#include <filesystem>

#include <buffer_pool/buffer_pool.h>
#include <transform/fixed_page_provider.h>

namespace sdb {

class Store : public IStore {
public:
	explicit Store(const size_t frame_count, const std::filesystem::path& path):
		buffer_pool_(bp::make_buffer_pool(frame_count)),
		path_(path)
	{}

	~Store() override = default;

	tb::TablePtr connect_table(const std::filesystem::path & name, SchemaPtr schema) override {
		bp::FilePtr file;
		if (std::filesystem::exists(path_ / name)) {
			file = bp::File::open_file(path_ / name);
		} else {
			file = bp::File::create_file(path_ / name);
		}

		return tb::make_table(buffer_pool_, file, make_fixed_page_provider(std::move(schema)));
	}

private:
	bp::BufferPoolPtr buffer_pool_;
	const std::filesystem::path path_;
};

StorePtr make_store(size_t frame_count, const std::filesystem::path& path) {
	return std::make_shared<Store>(frame_count, path);
}

} // namespace sdb