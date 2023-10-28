//
// Created by focus on 10/18/23.
//

#include "database.h"
#include "store/store.h"

namespace sdb {

class Database : public IDatabase {
public:
	explicit Database(const std::filesystem::path& path, const size_t frame_count) :
		store_(make_store(frame_count, path))
	{}

	~Database() override = default;

	void add_table() override {

	}

	void insert_row() override {

	}

	void get_row() override {

	}

private:
	StorePtr store_;
};

std::shared_ptr<IDatabase> make_database(const std::filesystem::path& path, const size_t frame_count) {
	return std::make_shared<Database>(path, frame_count);
}

} // namespace sdb