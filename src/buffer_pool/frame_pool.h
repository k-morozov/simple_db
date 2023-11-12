//
// Created by focus on 10/18/23.
//

#pragma once

#include <memory>

#include <buffer_pool/file.h>

namespace sdb::bp {

class IFramePool;
using FramePoolPtr = std::shared_ptr<IFramePool>;

FramePoolPtr make_frame_pool(size_t frame_count);

class IFramePool {
public:
	virtual ~IFramePool() = default;

	virtual std::pair<PageIndex, uint8_t *> acquire_frame(FilePtr file, PageIndex index) = 0;
	virtual void release_frame(PageIndex index) = 0;
};

} // namespace sdb::bp
