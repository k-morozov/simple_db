//
// Created by focus on 2/10/24.
//

#include "log.h"

#include <iostream>

namespace sdb {

std::ostream& operator<<(std::ostream& stream, const LogLevel level) {
	stream << "[";
	switch (level) {
		case LogLevel::DEBUG:
			stream << "DEBUG";
			break;
		case LogLevel::INFO:
			stream << "INFO";
			break;
		case LogLevel::ERROR:
			stream << "ERROR";
			break;
	}
	stream << "]\t";
	return stream;
};

Log::Log(LogLevel level): level_(level) {
	get_stream() << level_;
}

std::ostream &sdb::Log::get_stream() {
	if (level_ == LogLevel::ERROR) {
		return std::cerr;
	}
	return std::cout;
}

Log::~Log() noexcept {
	try {
		get_stream() << std::endl;
	} catch (...) {
		// @TODO nothing
	}
}


} // namespace sdb