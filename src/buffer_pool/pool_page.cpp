//
// Created by focus on 10/18/23.
//

#include "pool_page.h"
#include <utils/utils.h>

namespace sdb::bp {

PoolPage::PoolPage(FramePoolPtr owner, PageIndex index, uint8_t* data) :
	owner_(std::move(owner)),
	index_(index),
	data_frame_(data)
{}

uint8_t *PoolPage::get_data() {
	return data_frame_;
}

PoolPagePtr PoolPage::make_page(FramePoolPtr owner, PageIndex index, uint8_t *data) {
	return utils::ConstructFromProtected<PoolPage>::make(std::move(owner), index, data);
}

} // namespace sdb::bp