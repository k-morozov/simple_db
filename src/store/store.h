//
// Created by focus on 10/18/23.
//

#pragma once

#include <filesystem>
#include <memory>

#include <table/table.h>
#include <table/schema.h>

namespace sdb {

class IStore;
using StorePtr = std::shared_ptr<IStore>;

StorePtr make_store(size_t frame_count, const std::filesystem::path& path);

class IStore {
public:
	virtual ~IStore() = default;

	virtual tb::TablePtr connect_table(const std::filesystem::path & name, SchemaPtr schema) = 0;

	virtual bool exists(const std::filesystem::path & name) = 0;
	virtual void drop(const std::filesystem::path & name) = 0;
};

} // namespace sdb
