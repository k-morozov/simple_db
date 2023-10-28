//
// Created by focus on 10/18/23.
//

#pragma once

#include <memory>

namespace sdb::utils {

template<class T>
class ConstructFromProtected : public T {
public:
	template<typename ...Args>
	explicit ConstructFromProtected(Args&& ...args) : T(std::forward<Args>(args)...)
	{}

	template<typename ...Args>
	static std::shared_ptr<ConstructFromProtected<T>> make(Args&& ...args) {
		return std::make_shared<ConstructFromProtected<T>>(std::forward<Args>(args)...);
	}
};

} // namespace sdb