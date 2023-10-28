//
// Created by focus on 10/18/23.
//

#pragma once

#include <memory>

#include <buffer_pool/pool_page.h>

namespace sdb::bp {

class IBufferPool;
using BufferPoolPtr = std::shared_ptr<IBufferPool>;

BufferPoolPtr make_buffer_pool(size_t frame_count);

class IBufferPool {
public:
	virtual ~IBufferPool() = default;

	virtual PoolPagePtr get_page(FilePtr file, PageIndex index) = 0;
};

} // namespace sdb::bp
