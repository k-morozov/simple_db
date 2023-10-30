//
// Created by focus on 10/30/23.
//
#include <gtest/gtest.h>

#include <variant>
#include <unordered_map>

#include <store/store.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.
	EXPECT_EQ(7 * 6, 42);
}

TEST(SimpleTest, BasicInsertion) {
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
	for(size_t i=0; i<1; i++) {
		auto row = sdb::tb::Row{
				i,
				i % 2,
				100 + i,
		};
		const auto row_id = table->insert_row(row);
		expected_row_ids.insert({row_id, row});
	}

	for(const auto& [row_id, expected_row] : expected_row_ids) {
		const auto actual_row = table->get_row(row_id);
		ASSERT_EQ(actual_row, expected_row);
	}
}