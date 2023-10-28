//
// Created by focus on 10/19/23.
//

#pragma once

#include <memory>

namespace sdb::bp {

constexpr size_t PageSize = 4096;

class PoolPage;
using PoolPagePtr = std::shared_ptr<PoolPage>;
using PageIndex = size_t;

} // namespace sdb::bp
