//
// Created by focus on 2/14/24.
//

#pragma once

#include <tx/types.h>

#include <ostream>
#include <unordered_map>
#include <map>

namespace sdb::tx {

class Storage final {
public:
	struct TagValue {
		TxID txid;
		Value value;
	};
	using HistoryValues = std::map<Timestamp, TagValue>;

	const std::pair<const Timestamp, TagValue>* get_last(const Key& k) const;

	void apply(TxID txid, Timestamp commit_ts, Key key, Value value);

private:
	std::unordered_map<Key, HistoryValues> data_;
};

std::ostream& operator<<(std::ostream& os, const Storage::TagValue& tag);

} // namespace sdb::tx
