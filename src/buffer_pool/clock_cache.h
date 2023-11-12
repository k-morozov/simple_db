//
// Created by focus on 11/11/23.
//

#pragma once


#include <cstdlib>
#include <vector>
#include <unordered_map>

#include "buffer_pool/fwd.h"

namespace sdb::bp {

template<class TKey, class TValue>
class ClockCache {
public:
	explicit ClockCache(size_t cache_size) : capacity_(cache_size) {
		buffer_descriptors_layer_.resize(capacity_);
		for(size_t i=0; i<capacity_; i++) {
			buffer_descriptors_layer_[i] = BufferDescriptor{};
		}
	}
	~ClockCache() = default;

	std::pair<TValue, bool> get(const TKey& key) {
		if (!buffer_table_layer_.contains(key)) {
			return std::make_pair(0, false);
		}
		const BufferID buf_id = buffer_table_layer_[key];
		buffer_descriptors_layer_[buf_id].usage++;

		return std::make_pair(buf_id, true);
	}

	TValue put(const TKey& key) {
		for(;;increase()) {
			if (buffer_descriptors_layer_[cursor_].lock) {
				continue;
			}
			if (buffer_descriptors_layer_[cursor_].usage == 0) {
				break;
			}
			buffer_descriptors_layer_[cursor_].usage--;
		}

		const TKey old_key = buffer_descriptors_layer_[cursor_].tag;
		buffer_table_layer_.erase(old_key);

		buffer_descriptors_layer_[cursor_].tag = key;
		buffer_descriptors_layer_[cursor_].usage = 1;

		buffer_table_layer_[key] = cursor_;

		TValue result = cursor_;
		increase();

		return result;
	}

	void lock(const TValue index) {
		assert(index < capacity_);
		buffer_descriptors_layer_[index].lock = true;
	}

	void unlock(const TValue index) {
		assert(index < capacity_);
		buffer_descriptors_layer_[index].lock = false;
	}

private:
	using BufferID = size_t;

	struct BufferDescriptor final {
		TKey tag;
		size_t usage{0};
		bool lock{false};
	};

	std::unordered_map<TKey, BufferID> buffer_table_layer_;
	std::vector<BufferDescriptor> buffer_descriptors_layer_;

	const size_t capacity_;
	size_t cursor_{0};

	void increase() {
		cursor_++;
		if (cursor_ == capacity_) {
			cursor_ = 0;
		}
	}
};

} // namespace sdb


