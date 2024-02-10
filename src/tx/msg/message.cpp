//
// Created by focus on 2/10/24.
//

#include "message.h"

#include <ostream>

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

} // namespace sdb::tx