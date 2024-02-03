//
// Created by focus on 10/21/23.
//

#pragma once

#include <cassert>

namespace sdb::bp {

struct Frame final {
	uint8_t* data;
	FilePtr owner;
	PageIndex page_index;
	size_t ref_count;

	void dump_data() {
		if (owner) {
			owner->write(page_index, data);
		}
	}

	void upload_data() {
		assert(owner);
		owner->read(page_index, data);
	}
};

} // namespace sdb::bp