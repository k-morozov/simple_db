//
// Created by focus on 10/18/23.
//

#include "buffer_pool.h"
#include <buffer_pool/frame_pool.h>

namespace sdb::bp {

class BufferPool final : public IBufferPool {
public:
	explicit BufferPool(size_t frame_count) :
		frame_pool_(make_frame_pool(frame_count))
	{}

	~BufferPool() override = default;

	PoolPagePtr get_page(FilePtr file, PageIndex index) override {
		const auto [frame_index, page_data] = frame_pool_->acquire_frame(std::move(file), index);
		return PoolPage::make_page(frame_pool_, frame_index, page_data);
	}

private:
	FramePoolPtr frame_pool_;
};

BufferPoolPtr make_buffer_pool(const size_t frame_count) {
	return std::make_shared<BufferPool>(frame_count);
}

} // namespace sdb::bp