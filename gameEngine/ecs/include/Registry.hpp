#pragma once

#include <memory>
#include "SparseSet.hpp"

namespace ecs
{
    /**
     * Forward declaration of the View class, since it needs Registry to exist
     */
    template <typename... Components>
    class View;

    /**
     * Core of the ECS. It is the class that actually owns all of the entities and their components.
     * It stores the actual component data in pools (interfaced SparseSets) that it manages.
     * It allows :
     *  - Registering new components (given they exist at compile time)
     *  - Creating and deleting entities
     *  - Adding, accessing, and removing components
     * Its main advantage is that it defines entity lifetime and keeps data secure in one place.
     */
    class Registry
    {
    private:
        /**
         * View needs to be a friend class to access the component pools
         */
        template <typename... Components>
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

        /**
         * Indexing utilitary that gives each new type a unique id that is shared across all Registry instances.
         * It also registers the component names so that _component_names[index] = "name".
         *
         * This function is not meant to be called in public methods, use get_component_index() instead.
         * @param type_name Name given to the type
         * @return Next component index
         */
        static size_t next_component_index(std::string type_name)
        {
            static size_t index = 0;
            _component_names.push_back(type_name);
            return index++;
        }

        /**
         * Returns the unique index assigned to the given type.
         * It is computed at compile time, hence the static specifier.
         * @tparam T Type of element
         * @return Unique index of the templated type
         */
        template <typename T>
        static size_t get_component_index()
        {
            static size_t index = next_component_index(typeid(T).name());
            return index;
        }

        /**
         * Useful utilitary that does the same as get_component_id, only if the given type is not yet registered, it is added to the Registry.
         * @tparam T Type of element
         * @return Unique index of the templated type
         */
        template <typename T>
        size_t get_or_register_component_index()
        {
            size_t index = get_component_index<T>();

            if (index >= _component_pools.size())
                register_component<T>();

            return index;
        }

        /**
         * Returns a pointer to the templated type's corresponding component pool.
         *
         * If component is not registered, this returns nullptr.
         * @tparam T Type of component
         * @return Pointer to templated type's pool or nullptr
         */
        template <typename T>
        ISparseSet *get_component_pool_ptr()
        {
            size_t index = get_component_index<T>();

            if (!_component_pools[index])
                return nullptr;

            return _component_pools[index].get();
        }

        /**
         * Returns a reference to the templated type's corresponding component pool by performing a downcast operation on it.
         *
         * Calling this function on an unregistered type leads to undefined behaviour.
         * @tparam T Type of component
         * @return Reference to underlying SparseSet of templated type
         */
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

        /**
         * Simple utilitary to completely clear out the Registry.
         */
        void reset()
        {
            _deleted_ids.clear();
            _names.clear();
            _component_pools.clear();
            _component_names.clear();
            _max_entities = 0;
        };

        /**
         * Registers a new entity and its name to the Registry. If possible, old indexes will be recycled to save space.
         * @param name Name of the entity (can be duplicate)
         * @return Index of newly registered entity
         */
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

        /**
         * Returns a reference to the name at entity index.
         *
         * If index is invalid, returns an empty string.
         * @param entity Entity index
         * @return Reference to entity name
         */
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

        /**
         * Removes an entity's components from the pools and adds its index to the list of deleted indexes for recycling.
         * @param entity Entity index
         */
        void delete_entity(size_t entity)
        {
            ECS_ASSERT_VALID_ENTITY(entity);
            ECS_ASSERT_ALIVE_ENTITY(entity);

            for (auto &pool : _component_pools)
                pool.get()->unset(entity);

            _names.unset(entity);

            _deleted_ids.push_back(entity);
        }

        /**
         * Registers a new component to the Registry.
         *
         * If component is already registered, does nothing.
         * @tparam T Type of component
         */
        template <typename T>
        void register_component()
        {
            size_t index = get_component_index<T>();

            if (index >= _component_pools.size())
                _component_pools.resize(index + 1);

            if (_component_pools[index])
            {
                ECS_INFO("component already registered.");
                return;
            }

            _component_pools[index] = std::make_unique<SparseSet<T>>();
        }

        /**
         * Adds a new component to the given entity. If a component of same type is already present, it is overwritten.
         *
         * If the component is not yet registered, it is added to the Registry. It is better to register components first for readability's sake.
         * @tparam T Type of component
         * @tparam Args... Arg types
         * @param entity Entity index
         * @param component Component data
         * @return Reference to newly added component
         */
        template <typename T, typename... Args>
        T &add(size_t entity, Args &&...args)
        {
            ECS_ASSERT_VALID_ENTITY(entity);
            ECS_ASSERT_ALIVE_ENTITY(entity);

            get_or_register_component_index<T>();
            SparseSet<T> &pool = get_component_pool<T>();

            return pool.set(entity, T{std::forward<Args>(args)...});
        }

        /**
         * Gets a reference to the component attached to the given entity.
         *
         * If the given component is unregistered, the entity is invalid, or it is missing given component type, this function has undefined behavior.
         * @tparam T Type of component
         * @param entity Entity index
         * @return Reference to the component attached to entity
         */
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

        /**
         * Gets a pointer to the component attached to the given entity. If entity is invalid or does not have such component, returns nullptr.
         * @tparam T Type of component
         * @param entity Entity index
         * @return Pointer to entity's attached component
         */
        template <typename T>
        T *get_ptr(size_t entity)
        {
            ECS_ASSERT_VALID_ENTITY(entity);
            ECS_ASSERT_ALIVE_ENTITY(entity);

            SparseSet<T> &pool = get_component_pool<T>();
            return pool.get(entity);
        }

        /**
         * Removes a component of given type from the entity if one is attached.
         * @tparam T Type of component
         * @param entity Entity index
         */
        template <typename T>
        void remove(size_t entity)
        {
            ISparseSet *pool = get_component_pool_ptr<T>();

            if (!pool)
            {
                ECS_INFO("component [" << typeid(T).name() << "] is not registered.");
                return;
            }

            if (!pool->contains_index(entity))
            {
                ECS_INFO("entity has no component [" << _component_names[get_component_index<T>()] << "].");
                return;
            }

            pool->unset(entity);
            ECS_INFO("[" << _component_names[get_component_index<T>()] << "] component removed from \"" << get_entity_name(entity) << "\".");
        }

        /**
         * Returns true if the given entity has every one of the given components.
         * @tparam Ts Types of desired components
         * @param entity Entity index
         */
        template <typename... Ts>
        bool has(size_t entity)
        {
            if (!is_alive(entity))
                return false;

            return (has_one<Ts>(entity) && ...);
        }

        /**
         * Returns true if the given entity has the given components.
         * @tparam T Type of desired component
         * @param entity Entity index
         */
        template <typename T>
        bool has_one(size_t entity)
        {
            ISparseSet *pool = get_component_pool_ptr<T>();
            return pool && pool->contains_index(entity);
        }

        /**
         * Returns true if the given entity has any of the given components.
         * @tparam Ts Types of desired components
         * @param entity Entity index
         */
        template <typename... Ts>
        bool has_any(size_t entity)
        {
            return (has<Ts>(entity) || ...);
        }

        /**
         * Returns a View object to freely iterate over the elements of the Registry based on the components they own. The chosen types are set using templates. See View.hpp for more information about this class.
         * @tparam Components Types of desired components
         */
        template <typename... Components>
        View<Components...> view();

        /**
         * Returns true if entity is valid and not deleted.
         * @param entity Entity index
         */
        bool is_alive(size_t entity) const
        {
            return (entity == null_index || entity >= _max_entities || std::find(_deleted_ids.begin(), _deleted_ids.end(), entity) == _deleted_ids.end());
        }

        /**
         * Returns the number of registed components in the Registry.
         */
        size_t pool_count() const
        {
            return _component_pools.size();
        }
    };

} // namespace ecs

// Need to define this function after View class is implemented.

#include "View.hpp"

namespace ecs
{
    template <typename... Components>
    View<Components...> Registry::view()
    {
        return View<Components...>(this);
    }
} // namespace ecs