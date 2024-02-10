//
// Created by focus on 2/10/24.
//

#pragma once

#include <cstdint>
#include <ostream>

namespace sdb {

enum class LogLevel: uint8_t {
	DEBUG,
	INFO,
	ERROR
};

class Log final {
public:
	explicit Log(LogLevel);

	template<class T>
	Log& operator<<(T&& text) {
		get_stream() << text;
		return *this;
	}

	~Log() noexcept;
private:
	const LogLevel level_;

	std::ostream& get_stream();
};

#define LOG_DEBUG Log(LogLevel::DEBUG)
#define LOG_INFO Log(LogLevel::INFO)
#define LOG_ERROR Log(LogLevel::ERROR)

} // namespace sdb