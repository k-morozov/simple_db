//
// Created by focus on 10/18/23.
//

#pragma once

#include <filesystem>
#include <memory>

namespace sdb {

class IDatabase;
std::shared_ptr<IDatabase> make_database(const std::filesystem::path& path, size_t frame_count);

class IDatabase {
public:
	virtual ~IDatabase() = default;

	virtual void add_table() = 0;
	virtual void insert_row() = 0;
	virtual void get_row() = 0;
};

} // namespace sdb