//
// Created by focus on 2/11/24.
//

#pragma once

#include <random>

namespace sdb {

class GeneratorRandom final {
public:
	using Type = unsigned int;
	GeneratorRandom() = default;
	Type generate(Type from, Type to);
private:
	std::random_device rd;
	std::mt19937 generator_{rd()};
};

} // namespace sdb