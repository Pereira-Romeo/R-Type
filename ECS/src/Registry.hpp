#pragma once

#include <memory>
#include "SparseSet.hpp"

namespace ECS
{
    template <typename... Components>
    class View;

    class Registry
    {
    private:
        template <typename...>
        friend class View;

        /**
         * List of available ids for entities (in order to avoid allocating more)
         */
        std::vector<size_t> _deleted_ids;

        /**
         * Names attached to each entity id, mapping [id -> name].
         */
        SparseSet<std::string> _names;

        /**
         * Pointers to component pools.
         */
        std::vector<std::unique_ptr<ISparseSet>> _component_pools;

        /**
         * For debug purposes, holds the names of all registered components in the Registry.
         */
        static inline std::vector<std::string> _component_names;

        /**
         * Maximum number of entities to be handled by the Registry.
         */
        size_t _max_entities = 0;

        static size_t next_component_index(std::string type_name)
        {
            static size_t index = 0;
            _component_names.push_back(type_name);
            return index++;
        }

        template <typename T>
        static size_t get_component_index()
        {
            static size_t index = next_component_index(typeid(T).name());
            return index;
        }

        template <typename T>
        size_t get_or_register_component_index()
        {
            size_t index = get_component_index<T>();

            if (index >= _component_pools.size())
                register_component<T>();

            return index;
        }

        template <typename T>
        ISparseSet *get_component_pool_ptr()
        {
            size_t index = get_component_index<T>();

            if (!_component_pools[index])
                return nullptr;

            return _component_pools[index].get();
        }

        template <typename T>
        SparseSet<T> &get_component_pool()
        {
            ISparseSet *iptr = get_component_pool_ptr<T>();
            ECS_ASSERT(iptr, "component is not registered in this registry.");
            SparseSet<T> *ptr = static_cast<SparseSet<T> *>(iptr);
            return *ptr;
        }

#define ECS_ASSERT_REGISTERED_COMPONENT(type) \
    ECS_ASSERT(get_component_pool_ptr<T>(), "component is not registered in this registry.");
#define ECS_ASSERT_VALID_ENTITY(entity) \
    ECS_ASSERT(entity < _max_entities, "index is out of bounds.");
#define ECS_ASSERT_ALIVE_ENTITY(entity) \
    ECS_ASSERT(is_alive(entity), "entity is inactive and up for recycling.");

    public:
        Registry() = default;
        ~Registry()
        {
            reset();
        }

        void reset()
        {
            _deleted_ids.clear();
            _names.clear();
            _component_pools.clear();
            _component_names.clear();
            _max_entities = 0;
        };

        size_t create_entity(std::string name)
        {
            size_t index;

            if (_deleted_ids.empty())
            {
                index = _max_entities++;
            }
            else
            {
                index = _deleted_ids.back();
                _deleted_ids.pop_back();
            }

            _names.set(index, name);

            return index;
        }

        std::string &get_entity_name(size_t entity)
        {
            ECS_ASSERT_VALID_ENTITY(entity);
            ECS_ASSERT_ALIVE_ENTITY(entity);

            static std::string empty = "";
            std::string *name = _names.get(entity);

            if (name)
                return *name;

            return empty;
        }

        void delete_entity(size_t entity)
        {
            ECS_ASSERT_VALID_ENTITY(entity);
            ECS_ASSERT_ALIVE_ENTITY(entity);

            for (auto &pool : _component_pools)
                pool.get()->unset(entity);

            _names.unset(entity);

            _deleted_ids.push_back(entity);
        }

        template <typename T>
        void register_component()
        {
            size_t index = get_component_index<T>();

            if (index >= _component_pools.size())
            {
                _component_pools.resize(index + 1);
                _component_names.resize(index + 1);
            }

            _component_pools[index] = std::make_unique<SparseSet<T>>();
            _component_names[index] = typeid(T).name();
        }

        template <typename T>
        T &add(size_t entity, T &&component = {})
        {
            ECS_ASSERT_VALID_ENTITY(entity);
            ECS_ASSERT_ALIVE_ENTITY(entity);

            get_or_register_component_index<T>();
            SparseSet<T> &pool = get_component_pool<T>();

            return pool.set(entity, component);
        }

        template <typename T>
        T &get(size_t entity)
        {
            ECS_ASSERT_VALID_ENTITY(entity);
            ECS_ASSERT_ALIVE_ENTITY(entity);
            ECS_ASSERT_REGISTERED_COMPONENT(T);

            SparseSet<T> &pool = get_component_pool<T>();
            T *component = pool.get(entity);

            ECS_ASSERT(component, "entity \"" << get_entity_name(entity) << "\" is missing component [" << _component_names[get_component_index<T>()] << "].");

            return *component;
        }

        template <typename T>
        T *get_ptr(size_t entity)
        {
            ECS_ASSERT_VALID_ENTITY(entity);
            ECS_ASSERT_ALIVE_ENTITY(entity);

            SparseSet<T> &pool = get_component_pool<T>();
            return pool.get(entity);
        }

        template <typename T>
        void remove(size_t entity)
        {
            ECS_ASSERT_VALID_ENTITY(entity);
            ECS_ASSERT_ALIVE_ENTITY(entity);
            ECS_ASSERT_REGISTERED_COMPONENT(T);

            SparseSet<T> &pool = get_component_pool<T>();

            if (!pool.get(entity))
            {
                ECS_INFO("entity has no component [" << _component_names[get_component_index<T>()] << "].");
                return;
            }

            pool.unset(entity);
            ECS_INFO("[" << _component_names[get_component_index<T>()] << "] component removed from \"" << get_entity_name(entity) << "\".");
        }

        template <typename... Ts>
        bool has(size_t entity)
        {
            if (!is_alive(entity))
                return false;

            return (has_one<Ts>(entity) && ...);
        }

        template <typename T>
        bool has_one(size_t entity)
        {
            ISparseSet *pool = get_component_pool_ptr<T>();
            return pool && pool->contains_index(entity);
        }

        template <typename... Ts>
        bool has_any(size_t entity)
        {
            return (has<Ts>(entity) || ...);
        }

        template <typename... Components>
        View<Components...> view();

        bool is_alive(size_t entity) const
        {
            return (entity == null_index || entity >= _max_entities || std::find(_deleted_ids.begin(), _deleted_ids.end(), entity) == _deleted_ids.end());
        }

        size_t pool_count() const
        {
            return _component_pools.size();
        }
    };
}

#include "View.hpp"

namespace ECS
{
    template <typename... Components>
    View<Components...> Registry::view()
    {
        return View<Components...>(this);
    }
}