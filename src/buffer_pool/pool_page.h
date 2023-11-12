//
// Created by focus on 10/18/23.
//

#pragma once

#include <memory>

#include <buffer_pool/frame_pool.h>
#include <buffer_pool/fwd.h>

namespace sdb::bp {

class PoolPage {
public:
	static PoolPagePtr make_page(FramePoolPtr owner, PageIndex index, uint8_t* data);

	~PoolPage();

	uint8_t* get_data();

private:
	FramePoolPtr pool_;
	const PageIndex frame_index_;
	uint8_t* data_frame_;

protected:
	explicit PoolPage(FramePoolPtr owner, PageIndex index, uint8_t* data);
};

} // namespace sdb::bp