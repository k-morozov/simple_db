//
// Created by focus on 10/24/23.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace sdb {

enum class Type {
	boolean,
	uint64_t,
};

struct Column final {
	std::string name;
	Type type;
	std::size_t length;
};

using Schema = std::vector<Column>;
using SchemaPtr = std::shared_ptr<Schema>;

} // namespace sdb