//
// Created by focus on 10/21/23.
//

#pragma once

namespace sdb::bp {

struct Frame final {
	uint8_t* data;
	FilePtr owner;
	PageIndex index;
	size_t ref_count;
};

} // namespace sdb::bp