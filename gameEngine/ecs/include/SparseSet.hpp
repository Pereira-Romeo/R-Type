#pragma once

#include <algorithm>
#include <limits>
#include "ISparseSet.hpp"

namespace ecs
{

    /**
     * Null element, not reachable
     */
    constexpr std::size_t null_index = std::numeric_limits<size_t>::max();

    /**
     * Storage class to be used in the ECS. Its main advantages are :
     *  - that it owns all of the components, thus avoiding ownership troubles. Getting components is done through using an entity id
     *  - it allows for O(1) lookups, adding and deletion
     *  - it keeps actual data contiguous, making it cache friendly
     */
    template <typename T>
    class SparseSet : public ISparseSet
    {
    private:
        /**
         *List of indexes. indexes are stocked at the right spot for entities
         */
        std::vector<size_t> _sparse_list;

        /**
         * list of corresponding indexes, making it possible to look up splist index from dlist
         */
        std::vector<size_t> _dense_indexes;

        /**
         * Actual list that holds the objects
         */
        std::vector<T> _dense_list;

        /**
         * Max size of the sparse set
         */
        size_t _capacity = 0;

        /**
         * Gets the dense index attached to the corresponding entity index.
         *
         * If index is out of bounds or no component is attached to the entity, returns null index.
         * @param index Entity index
         * @return Corresponding index from _sparse_list or null_index
         */

        inline size_t get_dense_index(size_t index) const
        {
            if (index >= _capacity)
                return null_index;
            return _sparse_list[index];
        }

        /**
         * Adds a new dense index to the sparse set or reuses an unused spot.
         *
         * This should be used only when adding a new component, see set_dense_index() for direct modification.
         * @param index Index to be added
         */
        inline void add_dense_index(size_t index)
        {
            if (_dense_list.size() < _dense_indexes.size())
                _dense_indexes[_dense_list.size()] = index;
            else
                _dense_indexes.push_back(index);
        }

        /**
         * Modifies an existing dense index without adding a new one to the list.
         * @param index Entity index
         * @param item_index New value to be set at index
         */
        inline void set_dense_index(size_t index, size_t item_index)
        {
            size_t dense_index = get_dense_index(index);
            if (dense_index != null_index)
                _dense_indexes[dense_index] = item_index;
        }

        /**
         * Sets a value at given index in the sparse list
         * @param index Entity index
         * @param item_index Value to be set at index
         */
        inline void set_sparse_index(size_t index, size_t item_index)
        {
            if (index >= _sparse_list.size()) {
                _sparse_list.resize(index + 1, null_index);
                _capacity = index + 1;
            }
            _sparse_list[index] = item_index;
        }

    public:
        SparseSet() = default;
        ~SparseSet()
        {
            clear();
        }

        /**
         * Clears any and all data from the SparseSet.
         */
        inline void clear() override
        {
            _sparse_list.clear();
            _dense_indexes.clear();
            _dense_list.clear();
            _capacity = 0;
        }

        /**
         * Returns the number of components currently held within the SparseSet.
         * @return Number of elements
         */
        inline size_t size() const override
        {
            return _dense_list.size();
        }

        /**
         * Returns the maximum number of elements that can be added to the SparseSet.
         * @return Maximum number of elements
         */
        inline size_t capacity() const { return _capacity; }

        /**
         * Adds or replace an element V from _dense_list at entity index.
         * @param index Entity index at which to add the component
         * @param item Item to be added at index
         */
        template <typename V>
        T &set(size_t index, V &&item)
        {
            size_t dense_index = get_dense_index(index);
            if (dense_index != null_index)
            {
                _dense_list[dense_index] = std::forward<V>(item);
                return _dense_list[dense_index];
            }
            else
            {
                set_sparse_index(index, _dense_list.size());
                add_dense_index(index);
                _dense_list.push_back(std::forward<V>(item));
                return _dense_list.back();
            }
        }

        /**
         * Cleanly removes a compoment from an entity using a move and pop method in order to keep memory contiguous and avoid memory reallocation issues.
         * @param to_delete Entity id owning the element to remove
         */
        void unset(size_t to_delete) override
        {
            if (_dense_list.empty() || to_delete >= _capacity || _sparse_list[to_delete] == null_index)
                return;

            size_t delete_index = get_dense_index(to_delete); /// equivalent to _sparse_list[to_delete]
            size_t move_index = _dense_list.size() - 1;       /// equivalent to _sparse_list[move_index]
            size_t to_move = _dense_indexes[move_index];      /// equivalent to entity id owning the last element of the dense_list

            if (delete_index != move_index)
            {
                // Move the last element over the one to delete
                _dense_list[delete_index] = std::move(_dense_list.back());

                // In the corresponding _dense_index, set the new value to be the entity owning the element we just moved
                set_dense_index(to_delete, to_move);

                // Update the moved entity's _sparse_index to reflect its component's new position
                set_sparse_index(to_move, delete_index);
            }

            // Update the removed element's _sparse_list index to null_index
            set_sparse_index(to_delete, null_index);

            _dense_indexes.pop_back();
            _dense_list.pop_back();
        }

        /**
         * Returns a pointer to the element associated with the entity id. If no component is attached or id is invalid, returns nullptr.
         * @param index Entity index
         * @return Pointer to resource
         */
        T *get(size_t index)
        {
            if (index >= _capacity || _sparse_list[index] == null_index)
                return nullptr;
            return &_dense_list[_sparse_list[index]];
        }

        /**
         * Returns true if a component is attached to the given entity id.
         * @param id Entity index
         */
        bool contains_index(size_t id) const override
        {
            return get_dense_index(id) != null_index;
        }

        /**
         * Get a copy of indexes of all entities that have components registered into the SparseSet.
         * @return Vector of entity ids
         */
        const std::vector<size_t> &get_index_list() const override
        {
            return _dense_indexes;
        }

        /**
         * Dump the SparseSet data onto a given stream. If this is used, the templated type should have an existing operator<< overload.
         * @param os Stream to be written to
         */
        std::ostream &print(std::ostream &os) const
        {
            os << "SPLIST INDEX\t | DLIST INDEX\t\t\t|" << std::endl;

            for (size_t idx = 0; auto &elem : _sparse_list)
            {
                os << "index : " << idx << "\t | element position : ";
                idx++;
                if (elem == null_index)
                {
                    os << "(null)" << " \t|";
                    os << std::endl;
                    continue;
                }
                os << elem << " \t|";
                os << "\tsplist index : " << _dense_indexes[elem];
                os << "\telement : " << _dense_list[elem];
                os << std::endl;
            }
            return os;
        }
    };

} // namespace ecs

template <typename T>
std::ostream &operator<<(std::ostream &os, const ecs::SparseSet<T> &set)
{
    set.print(os);
    return os;
}
