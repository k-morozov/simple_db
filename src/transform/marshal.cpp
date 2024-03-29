//
// Created by focus on 10/24/23.
//
#include "marshal.h"

#include <cassert>
#include <cstring>

namespace sdb {

namespace {

template<typename T>
void serialize_value(const T& src, uint8_t*& dest) {
	std::memcpy(dest, &src, sizeof(T));
	dest += sizeof(T);
}

template<typename T>
T deserialize_value(uint8_t*& src) {
	T result;
	std::memcpy(&result, src, sizeof(T));
	src += sizeof(T);
	return result;
}

struct RowVisit {
	explicit RowVisit(uint8_t*& data) : data_(data) {}

	template<typename T>
	void operator()(T value) {
		serialize_value<T>(std::move(value), data_);
	};
private:
	uint8_t*& data_;
};

size_t calculate_row_space(const SchemaPtr& schema) {
	std::size_t result{};
	for(const auto& column : *schema) {
		switch (column.type) {
			case Type::boolean:
			case Type::uint64_t:
				result += column.length;
				break;
			default:
				throw std::invalid_argument("unsupported column type");
		}
	}
	return result;
}

} // anonymous namespace

sdb::Marshal::Marshal(SchemaPtr schema) :
	schema_(std::move(schema)),
	fixed_row_space_(calculate_row_space(schema_))
{}

void Marshal::serialize_row(uint8_t* data, const tb::Row &row) {
	for (auto value : row) {
		uint8_t* old_data = data;
		std::visit(RowVisit{data}, value);
		assert(old_data != data);
	}
}

tb::Row Marshal::deserialize_row(uint8_t *data) const {
	tb::Row result_row;
	for (const auto& column : *schema_) {
		uint8_t* old_data = data;
		switch (column.type) {
			case Type::boolean:
			{
				const bool field = deserialize_value<bool>(data);
				result_row.emplace_back(field);
				break;
			}
			case Type::uint64_t:
			{
				const auto field = deserialize_value<uint64_t>(data);
				result_row.emplace_back(field);
				break;
			}
			default:
				throw std::runtime_error("unsupported type");
		}
		assert(old_data != data);
	}
	return result_row;
}


} // namespace sdb