//
// Created by focus on 2/10/24.
//

#include "message.h"

#include <ostream>
#include <tuple>

namespace sdb::tx {

std::ostream& operator<<(std::ostream& stream, const MessageType type) {
	switch (type) {
		case MessageType::MSG_UNDEFINED:
			stream << "MSG_UNDEFINED";
			break;
		case MessageType::MSG_START:
			stream << "MSG_START";
			break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Message& msg) {
	stream << "message "
			<< "[type=" << msg.type << "]"
		   	<< "[id=" << msg.id << "]"
		   	<< "[from " << msg.source << "]"
		   	<< "[to " << msg.destination << "]";
	return stream;
}

bool operator==(const Message& lhs, const Message& rhs) {
	return std::make_tuple(lhs.id, lhs.type, lhs.source, lhs.destination)
		==
		std::make_tuple(rhs.id, rhs.type, rhs.source, rhs.destination);
}

bool operator<(const Message& lhs, const Message& rhs) {
	return std::make_tuple(lhs.type, lhs.id, lhs.source, lhs.destination)
		   <
		   std::make_tuple(rhs.type, rhs.id, rhs.source, rhs.destination);
}


} // namespace sdb::tx