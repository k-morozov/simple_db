//
// Created by focus on 10/30/23.
//
#include <gtest/gtest.h>

#include <variant>
#include <unordered_map>

#include <store/store.h>

sdb::tb::Row create_row(size_t i) {
	return sdb::tb::Row{
			10u + i,
			static_cast<bool>(i % 2),
			100u + i,
	};
}

const auto schema = std::make_shared<sdb::Schema>(sdb::Schema{
	sdb::Column{
		.name="id",
		.type=sdb::Type::uint64_t,
		.length=8
	},
	sdb::Column{
			.name="status",
			.type=sdb::Type::boolean,
			.length=1
	},
	sdb::Column{
			.name="number",
			.type=sdb::Type::uint64_t,
			.length=8
	},
});

class TableFixture : public ::testing::Test {
protected:
	sdb::StorePtr store;
	sdb::tb::TablePtr table;
	std::string db_name;

	void connect() {
//		assert(!store->exists(get_db_name()));
		table = store->connect_table(get_db_name(), schema);
		assert(table);
	}
	void disconnect() {
		table.reset();
	}

public:
	void SetUp() override {
		store = sdb::make_store(128, "");
		generate_db_name();

		if (store->exists(get_db_name())) {
			store->drop(get_db_name());
		}
		connect();
	}

	void TearDown() override {
		disconnect();
	}

	void generate_db_name() {
		db_name = "test_db_" + std::to_string(rand());
	}

	std::string get_db_name() const {
		assert(!db_name.empty());
		return db_name;
	}

	void restart() {
		disconnect();
		connect();
	}
};

class TableParametrizedFixture :
		public TableFixture,
		public ::testing::WithParamInterface<std::tuple<int, int>>
{};

struct Params {
	size_t insert_count;
	size_t get_count;
};

std::tuple<size_t, size_t> ConvertFromParam(const Params& p) {
	return std::make_tuple(p.insert_count, p.get_count);
}

INSTANTIATE_TEST_SUITE_P(
		BasicInsertion,
		TableParametrizedFixture,
		::testing::Values(
				ConvertFromParam({
					.insert_count=10,
					.get_count=10}),
				ConvertFromParam({
					.insert_count=10,
					.get_count=100}),
				ConvertFromParam({
					.insert_count=100,
					.get_count=10}),
				ConvertFromParam({
					.insert_count=10,
					.get_count=10'000}),
				ConvertFromParam({
					.insert_count=10'000,
					.get_count=10}),
				ConvertFromParam({
					.insert_count=1'000,
					.get_count=1'000})
		));

struct ExpectedRow {
	sdb::tb::RowID row_id;
	sdb::tb::Row row;
};

TEST_P(TableParametrizedFixture, InsertAndGetRow) {
	std::vector<ExpectedRow> expected_row_ids;

	const auto [insert_count, get_count] = GetParam();

	for(size_t i=0; i<insert_count; i++) {
		auto row = create_row(i);
		const sdb::tb::RowID row_id = table->insert_row(row);
		expected_row_ids.push_back(ExpectedRow{
			.row_id=row_id,
			.row=row
		});
	}
	for(const auto& expected : expected_row_ids) {
		for(size_t i=0; i<get_count; i++) {
			const auto actual_row = table->get_row(expected.row_id);
			ASSERT_EQ(actual_row, expected.row);
		}
	}
}

TEST_P(TableParametrizedFixture, DISABLED_InsertAndGetRowWithRestart) {
	std::vector<ExpectedRow> expected_row_ids;

	const auto [insert_count, get_count] = GetParam();

	for(size_t i=0; i<insert_count; i++) {
		auto row = create_row(i);
		const sdb::tb::RowID row_id = table->insert_row(row);
		expected_row_ids.push_back(ExpectedRow{
				.row_id=row_id,
				.row=row
		});
	}

	restart();

	for(const auto& expected : expected_row_ids) {
		for(size_t i=0; i<get_count; i++) {
			const auto actual_row = table->get_row(expected.row_id);
			ASSERT_EQ(actual_row, expected.row);
		}
	}
}