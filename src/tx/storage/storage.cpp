//
// Created by focus on 2/14/24.
//

#include "storage.h"

#include <cassert>

namespace sdb::tx {

const std::pair<const Timestamp, Storage::TagValue> *Storage::get_last(const Key& key) const {
	if (!data_.contains(key))
		return nullptr;
	assert(!data_.at(key).empty());

	return &*std::prev(data_.at(key).end());
}

void Storage::apply(const TxID txid, const Timestamp commit_ts, const Key key, const Value value) {
	data_[key].insert(std::make_pair(commit_ts, TagValue{
		.txid=txid,
		.value=value
	}));
}

std::ostream& operator<<(std::ostream& os, const Storage::TagValue& tag) {
	os << "tag: txid=" << tag.txid
		<< ", value=" << tag.value;
	return os;
}

} // namespace sdb::tx
