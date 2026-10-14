#pragma once

#include <iostream>
#include <vector>

#ifndef ECS_ASSERT
	#define ECS_ASSERT(condition, msg) \
		if (!(condition)) { \
			std::cerr << "ecs error: " << msg << std::endl; \
			::abort(); \
		}
#endif
#ifndef ECS_INFO
	#ifdef ECS_INFO_ENABLED
		#define ECS_INFO(msg) std::cout << "ecs info: " << msg << "\n";
	#else
		#define ECS_INFO(msg);
	#endif
#endif

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec)
{
    for (auto &elem : vec)
        os << "\t" << elem;
    return os;
}
