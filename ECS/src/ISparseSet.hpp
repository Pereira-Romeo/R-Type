#pragma once

#include <cstddef>
#include "Debug.hpp"

namespace ECS
{

	/**
	 * Template class for the SparseSet.
	 * It is used to hold all component pools in a single vector in the Registry
	 */
    class ISparseSet
    {
    public:
		virtual ~ISparseSet() = default;
		virtual void unset(size_t id) = 0;
		virtual void clear() = 0;
		virtual size_t size() const = 0;
		virtual bool contains_index(size_t id) const = 0;
		virtual std::vector<size_t> get_index_list() const = 0;
    };

} // namespace ECS
