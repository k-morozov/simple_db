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

	std::pair<FrameIndex, bool> get(const TKey& key) {
		if (!buffer_table_layer_.contains(key)) {
			return std::make_pair(0, false);
		}
		const BufferID buf_id = buffer_table_layer_[key];
		buffer_descriptors_layer_[buf_id].chance = true;

		return std::make_pair(buffer_descriptors_layer_[buf_id].value, true);
	}

	FrameIndex put(const TKey& key, const TValue& value) {
		for(;;increase()) {
			if (buffer_descriptors_layer_[cursor_].lock) {
				continue;
			}
			if (buffer_descriptors_layer_[cursor_].chance == false) {
				break;
			}
			buffer_descriptors_layer_[cursor_].chance = false;
		}

		const TKey old_key = buffer_descriptors_layer_[cursor_].tag;
		buffer_table_layer_.erase(old_key);

		buffer_descriptors_layer_[cursor_].tag = key;
		buffer_descriptors_layer_[cursor_].value = value;
		buffer_descriptors_layer_[cursor_].chance = true;

		buffer_table_layer_[key] = cursor_;

		increase();
		return cursor_;
	}

	void lock(const FrameIndex index) {
		assert(index < capacity_);
		buffer_descriptors_layer_[index].lock = true;
	}

	void unlock(const FrameIndex index) {
		assert(index < capacity_);
		buffer_descriptors_layer_[index].lock = false;
	}

private:
	using BufferID = size_t;

	struct BufferDescriptor final {
		TKey tag;
		TValue value;

		bool chance{false};
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


