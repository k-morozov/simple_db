//
// Created by focus on 2/11/24.
//

#include "generator.h"

#include <random>
#include <cassert>

namespace sdb {

GeneratorRandom::Type GeneratorRandom::generate(const Type from, const Type to) {
	std::uniform_int_distribution<> distrib(
			static_cast<int>(from),
			static_cast<int>(to)
	);
	const auto res = distrib(generator_);
	assert(res>=0);
	return res;
};

} // namespace sdb