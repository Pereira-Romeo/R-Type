#pragma once

#include <array>
#include "Registry.hpp"

namespace ecs
{
    template <typename... Components>
    class View
    {
    private:
        Registry *_ecs;
        std::array<ISparseSet *, sizeof...(Components)> _pools;
        std::vector<ISparseSet *> _excluded_pools;
        const std::vector<size_t> *_driver;

        bool all_contain(std::size_t index)
        {
            return std::all_of(_pools.begin(), _pools.end(), [index](ISparseSet *set)
                               { return set->contains_index(index); });
        }

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

        class Iterator
        {
        private:
            View *_view;
            size_t _index;

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