#include <cassert>
#include <iostream>
#include <variant>
#include <unordered_map>

#include <store/store.h>

int main() {
	std::cout << "Hello, World!" << std::endl;
	auto store = sdb::make_store(5, "");
	auto schema = std::make_shared<sdb::Schema>(sdb::Schema{
		sdb::Column{
			.name="id",
			.type=sdb::Type::uint64_t,
			.length=4
		},
		sdb::Column{
			.name="status",
			.type=sdb::Type::boolean,
			.length=1
		},
		sdb::Column{
			.name="number",
			.type=sdb::Type::uint64_t,
			.length=4
		},
	});
	auto table = store->connect_table("test_db", schema);

	std::unordered_map<sdb::tb::RowID, sdb::tb::Row> expected_row_ids;
	for(size_t i=0; i<3; i++) {
		auto row = sdb::tb::Row{
				i,
				i % 2,
				100 + i,
		};
		const auto row_id = table->insert_row(row);
		expected_row_ids.insert({row_id, row});
	}

	for(const auto& [row_id, row_expected] : expected_row_ids) {
		const auto row = table->get_row(row_id);
		std::cout << "=== row:" << std::endl;
		for(const auto v : row) {
			std::visit([](auto& arg) -> void {
				std::cout << arg << std::endl;
			}, v);
		}
		assert(row == row_expected);
		std::cout << std::endl;
	}

	return 0;
}
