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
using FrameIndex = size_t;
using FileFD = int;
using PageID = std::pair<FileFD, PageIndex>;

} // namespace sdb::bp

namespace std
{

template <>
class hash<sdb::bp::PageID>
{
public:
	size_t operator()(const sdb::bp::PageID & page_id) const {
		return std::hash<sdb::bp::FileFD>()(page_id.first) ^ std::hash<sdb::bp::PageIndex>()(page_id.second);
	}
};

}

