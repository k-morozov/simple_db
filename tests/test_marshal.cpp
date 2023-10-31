//
// Created by focus on 10/31/23.
//
#include <gtest/gtest.h>

#include <variant>

#include <transform/marshal.h>

TEST(TestMarshal, MarshalOneRow) {
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
			sdb::Column{
					.name="some_id",
					.type=sdb::Type::uint64_t,
					.length=8
			},
			sdb::Column{
					.name="another id",
					.type=sdb::Type::uint64_t,
					.length=8
			},
	});

	sdb::Marshal marshal(schema);

	ASSERT_EQ(marshal.fixed_row_space(), 33);

	const auto expected_row = sdb::tb::Row{
			10u,
			true,
			100u,
			17u,
			123u
	};
	const size_t space = marshal.fixed_row_space();

	auto data = std::make_unique<uint8_t[]>(space);

	marshal.serialize_row(data.get(), expected_row);
	const auto actual_row = marshal.deserialize_row(data.get());

	ASSERT_EQ(expected_row, actual_row);

	ASSERT_EQ(std::get<2>(actual_row[0]), 10u);
	ASSERT_EQ(std::get<1>(actual_row[1]), true);
	ASSERT_EQ(std::get<2>(actual_row[2]), 100u);
	ASSERT_EQ(std::get<2>(actual_row[3]), 17u);
	ASSERT_EQ(std::get<2>(actual_row[4]), 123u);
}

TEST(TestMarshal, MarshalSomeRows) {
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

	sdb::Marshal marshal(schema);
	ASSERT_EQ(marshal.fixed_row_space(), 17);


	const size_t space = marshal.fixed_row_space();

	auto data = std::make_unique<uint8_t[]>(2 * space);

	const auto expected_rows = std::vector<sdb::tb::Row>{
		sdb::tb::Row{
			10u,
			true,
			100u,
		},
		sdb::tb::Row{
			12u,
			false,
			120u,
		}
	};
	marshal.serialize_row(data.get(), expected_rows[0]);
	marshal.serialize_row(data.get() + space, expected_rows[1]);

	const auto actual_row = marshal.deserialize_row(data.get());
	ASSERT_EQ(expected_rows[0], actual_row);

	const auto actual_second_row = marshal.deserialize_row(data.get() + space);
	ASSERT_EQ(expected_rows[1], actual_second_row);

}