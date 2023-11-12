//
// Created by focus on 10/18/23.
//

#include "frame_pool.h"

#include <cassert>
#include <vector>

#include <malloc.h>

#include "frame.h"
#include "buffer_pool/clock_cache.h"

namespace sdb::bp {

class FramePool : public IFramePool {
public:
	explicit FramePool(const size_t frame_count) :
		frames_(frame_count),
		cache_(frame_count)
	{
		data_ = reinterpret_cast<uint8_t *>(pvalloc(frame_count * PageSize));
		for(size_t index = 0 ; index < frame_count; index++) {
			frames_[index].data = data_ + index * PageSize;
		}
	}
	~FramePool() override {
		for(size_t internal_pool_index = 0 ; internal_pool_index < frames_.size(); internal_pool_index++) {
			dump_frame(internal_pool_index);
		}
		free(data_);
	}

	std::pair<FrameIndex, uint8_t *> acquire_frame(FilePtr file, const PageIndex page_index) override {
		FrameIndex frame_index;

		const auto key = std::make_pair(file->get_fd(), page_index);

		if (auto [index, found] = cache_.get(key); found) {
			frame_index = index;
			assert(frames_[frame_index].page_index == page_index);
			assert(frames_[frame_index].owner->get_fd() == file->get_fd());

		} else {
			const FrameIndex index_update_frame = cache_.put(key);
			dump_frame(index_update_frame);

			frame_index = index_update_frame;
			read_frame(file, frame_index, page_index);
		}

		++frames_[frame_index].ref_count;
		if (frames_[frame_index].ref_count == 1) {
			cache_.lock(frame_index);
		}

		return {frame_index, frames_[frame_index].data};
	}

	void release_frame(const FrameIndex index) override {
//		frames_[index].owner.reset();
		--frames_[index].ref_count;
		if (frames_[index].ref_count == 0) {
			cache_.unlock(index);
		}
	}

private:
	uint8_t * data_{nullptr};

	std::vector<Frame> frames_;
	ClockCache<PageID, FrameIndex> cache_;

	void dump_frame(const FrameIndex internal_pool_index) {
		assert(internal_pool_index < frames_.size());
		frames_[internal_pool_index].dump();
	}

	void read_frame(FilePtr file, const FrameIndex internal_pool_index, const PageIndex external_frame_index) {
		assert(internal_pool_index < frames_.size());

		frames_[internal_pool_index].owner = std::move(file);
		frames_[internal_pool_index].ref_count = 0;
		frames_[internal_pool_index].page_index = external_frame_index;

		frames_[internal_pool_index].upload();
	}
};

FramePoolPtr make_frame_pool(const size_t frame_count) {
	return std::make_shared<FramePool>(frame_count);
}

} // namespace sdb::bp