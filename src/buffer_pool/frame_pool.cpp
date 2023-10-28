//
// Created by focus on 10/18/23.
//

#include "frame_pool.h"

#include <vector>

#include <malloc.h>

#include "frame.h"

namespace sdb::bp {

class FramePool : public IFramePool {
public:
	explicit FramePool(size_t frame_count) :
		frames_(frame_count)
	{
		data_ = reinterpret_cast<uint8_t *>(pvalloc(frame_count * PageSize));
		for(size_t index = 0 ; index < frame_count; index++) {
			frames_[index].data = data_ + index * PageSize;
		}
	}
	~FramePool() override {
		free(data_);
	}

	std::pair<size_t, uint8_t *> acquire_frame(FilePtr file, PageIndex index) override {
		frames_[index].owner = file;
		frames_[index].ref_count++;
		return {index, frames_[index].data};
	}

	void release_frame(PageIndex index) override {
		frames_[index].owner.reset();
		frames_[index].ref_count--;
	}

private:
	uint8_t * data_{nullptr};
	std::vector<Frame> frames_;
};

FramePoolPtr make_frame_pool(const size_t frame_count) {
	return std::make_shared<FramePool>(frame_count);
}

} // namespace sdb::bp