//
// Created by focus on 10/18/23.
//

#include "pool_page.h"
#include <utils/utils.h>

namespace sdb::bp {

PoolPage::PoolPage(FramePoolPtr owner, PageIndex frame_index, uint8_t* data) :
	pool_(std::move(owner)),
	frame_index_(frame_index),
	data_frame_(data)
{}

uint8_t *PoolPage::get_data() {
	return data_frame_;
}

PoolPagePtr PoolPage::make_page(FramePoolPtr owner, PageIndex frame_index, uint8_t *data) {
	return utils::ConstructFromProtected<PoolPage>::make(std::move(owner), frame_index, data);
}

PoolPage::~PoolPage() {
	pool_->release_frame(frame_index_);
}

} // namespace sdb::bp