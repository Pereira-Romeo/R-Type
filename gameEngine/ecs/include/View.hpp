#pragma once

#include <array>
#include "Registry.hpp"

namespace ecs
{
    /**
     * Volatile class that allows easy iteration over the different entities contained in the Registry.
     * It allows:
     *  - Iteration over a Registry with components based selection
     *  - Exclusion of certain components (useful with Tags for example)
     * It is usually created from a Registry using the Registry::view method.
     */
    template <typename... Components>
    class View
    {
    private:
        /**
         * Pointer to the linked Registry.
         */
        Registry *_ecs;

        /**
         * Array containing the pointers to the desired component pools.
         */
        std::array<ISparseSet *, sizeof...(Components)> _pools;

        /**
         * Vector containing the pointers to the exluded pools.
         */
        std::vector<ISparseSet *> _excluded_pools;

        /**
         * Smallest list of indexes among the desired component pools. It is used as the reference list to compare against all the other pools.
         */
        const std::vector<size_t> *_driver;

        /**
         * Returns true if the given entity has all of the desired components.
         * @param index Entity index
         */
        bool all_contain(std::size_t index)
        {
            return std::all_of(_pools.begin(), _pools.end(), [index](ISparseSet *set)
                               { return set->contains_index(index); });
        }

        /**
         * Return true if the given entity has none of the exluded components.
         * @param index Entity index
         */
        bool not_excluded(std::size_t index)
        {
            if (_excluded_pools.empty())
                return true;
            return std::none_of(_excluded_pools.begin(), _excluded_pools.end(), [index](ISparseSet *set)
                                { return set->contains_index(index); });
        }

    public:
        View(Registry *ecs) : _ecs(ecs), _pools{ecs->get_component_pool_ptr<Components>()...}
        {
            static std::vector<size_t> empty;

            if (std::any_of(_pools.begin(), _pools.end(), [](ISparseSet *pool)
                            { return !pool; }))
            {
                ECS_INFO("view has invalid components");
                _driver = &empty;
                return;
            }

            ISparseSet *smallest = *std::min_element(_pools.begin(), _pools.end(), [](ISparseSet *a, ISparseSet *b)
                                             { return a->size() < b->size(); });

            ECS_ASSERT(smallest != *_pools.end(), "view is empty");

            _driver = &smallest->get_index_list();
        }

        /**
         * Sets the list of excluded components. Invalid components will be skipped.
         */
        template <typename... ExcludedComponents>
        View &without() &
        {
            for (ISparseSet *pool : {_ecs->get_component_pool_ptr<ExcludedComponents>()...})
            {
                if (pool)
                {
                    _excluded_pools.push_back(pool);
                }
            }
            return *this;
        }

        /**
         * Sets the list of excluded components. Invalid components will be skipped.
         */
        template <typename... ExcludedComponents>
        View without() &&
        {
            for (ISparseSet *pool : {_ecs->get_component_pool_ptr<ExcludedComponents>()...})
            {
                if (pool)
                {
                    _excluded_pools.push_back(pool);
                }
            }
            return std::move(*this);
        }

        /**
         * Iterator for the View class. It is a basic implementation of an iterator with minimal functionalities, to be mainly used in for auto loops.
         */
        class Iterator
        {
        private:
            /**
             * Pointer to linked View.
             */
            View *_view;

            /**
             * Internal index.
             */
            size_t _index;

            /**
             * Increases the index until the first element that meets the requirements or until the end of the list.
             */
            void next()
            {
                while (_index < _view->_driver->size() && !(_view->all_contain((*_view->_driver)[_index]) && _view->not_excluded((*_view->_driver)[_index])))
                    _index++;
            }

        public:
            Iterator(View *view, size_t index) : _view(view), _index(index)
            {
                next();
            }

            Iterator &operator++()
            {
                _index++;
                next();
                return *this;
            }

            size_t operator*() const
            {
                return (*_view->_driver)[_index];
            }

            bool operator!=(const Iterator &rhs) const
            {
                return this->_index != rhs._index;
            }
        };

        Iterator begin()
        {
            return Iterator(this, 0);
        }

        Iterator end()
        {
            return Iterator(this, _driver->size());
        }
    };

} // namespace ecs