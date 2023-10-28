#include <iostream>
#include <variant>

#include <store/store.h>

int main() {
	std::cout << "Hello, World!" << std::endl;
	auto store = sdb::make_store(5, "");
	auto schema = std::make_shared<sdb::Schema>(sdb::Schema{
		sdb::Column{
			.name="id",
			.type=sdb::Type::uint64_t,
			.length=0
		},
		sdb::Column{
			.name="status",
			.type=sdb::Type::boolean,
			.length=0
		},
		sdb::Column{
			.name="number",
			.type=sdb::Type::uint64_t,
			.length=0
		},
	});
	auto table = store->connect_table("test_db", schema);

	const auto row_id = table->insert_row(sdb::tb::Row{
			uint64_t{17},
			bool(true),
			uint64_t{10'000'033},
	});
	const auto row = table->get_row(row_id);

	for(const auto v : row) {
		std::visit([](auto& arg) -> void {
			std::cout << arg << std::endl;
		}, v);
	}
	return 0;
}
