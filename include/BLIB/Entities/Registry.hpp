#ifndef BLIB_ENTITIES_REGISTRY_HPP
#define BLIB_ENTITIES_REGISTRY_HPP

#include <BLIB/Containers/Any.hpp>
#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <BLIB/Entities/Component.hpp>
#include <BLIB/Entities/ComponentSet.hpp>
#include <BLIB/Entities/Entity.hpp>
#include <BLIB/Entities/Events.hpp>
#include <BLIB/Events/Dispatcher.hpp>

#include <algorithm>
#include <any>
#include <list>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace bl
{
namespace entity
{
/**
 * @brief Core class of the entity component system. This class stores and manages all entities and
 *        components. It is threadsafe and provides views that stay up to date as the registry
 *        evolves
 *
 * @ingroup Entities
 *
 */
class Registry {
    class ViewBase {
    public:
        template<typename... TComponents>
        struct DeductionDummy {};

        template<typename... Ts>
        ViewBase(Registry& registry, DeductionDummy<Ts...> _);

        virtual ~ViewBase();

        void makeDirty(Component::IdType cid);

    protected:
        bool isDirty() const;
        void makeClean();

    private:
        Registry& registry;
        const std::unordered_set<Component::IdType> componentTypes;
        bool dirty;

        friend class Registry;
    };

public:
    using EntityIterator      = std::unordered_set<Entity>::iterator;
    using ConstEntityIterator = std::unordered_set<Entity>::const_iterator;

    /**
     * @brief Construct a new Registry object
     *
     */
    Registry();

    /**
     * @brief Sets the event dispatcher to fire entity events through. Optional
     *
     * @param dispatcher The dispatcher to fire events through. Must remain in scope
     */
    void setEventDispatcher(bl::event::Dispatcher& dispatcher);

    /**
     * @brief Create a new Entity
     *
     * @return Entity The new Entity
     */
    Entity createEntity();

    /**
     * @brief Create a new Entity using the given identifier
     *
     * @param entity The id to use
     * @return True if the entity was created, false if already exists
     */
    bool createEntity(Entity entity);

    /**
     * @brief Returns whether or not the given entity exists
     *
     */
    bool entityExists(Entity entity) const;

    /**
     * @brief Destroys the given entity and all of its components
     *
     * @param entity The entity to destroy. No effect if it does not exist
     */
    void destroyEntity(Entity entity);

    /**
     * @brief Returns an iterator to the first Entity
     *
     */
    EntityIterator begin();

    /**
     * @brief Returns an iterator to the first Entity
     *
     */
    ConstEntityIterator begin() const;

    /**
     * @brief Returns an iterator to the last Entity
     *
     */
    EntityIterator end();

    /**
     * @brief Returns an iterator to the last Entity
     *
     */
    ConstEntityIterator end() const;

    /**
     * @brief Adds a component to the given entity if the entity exists and does not already have
     *        the component
     *
     * @tparam TComponent Type of component to add
     * @param entity The entity to add the component to
     * @param component The component to add
     * @return True if the component was added, false otherwise
     */
    template<typename TComponent>
    bool addComponent(Entity entity, const TComponent& component);

    /**
     * @brief Returns whether or not an entity has the given component
     *
     * @tparam TComponent The type of component to check for
     * @param entity The entity to check
     * @return True if the entity has the given component, false otherwise
     */
    template<typename TComponent>
    bool hasComponent(Entity entity) const;

    /**
     * @brief Returns a pointer to the given entity component
     *
     * @tparam TComponent The component type to access
     * @param entity The entity to get the component for
     * @return TComponent* Pointer to component or nullptr if not present or invalid entity
     */
    template<typename TComponent>
    TComponent* getComponent(Entity entity);

    /**
     * @brief Removes the given component from the given entity
     *
     * @tparam TComponent The type of component to remove
     * @param entity The entity to remove it from
     * @return True if the component was removed, false if not present or invalid entity
     */
    template<typename TComponent>
    bool removeComponent(Entity entity);

    /**
     * @brief An iterable view of entities with the given components. The view gets updated when the
     *        underlying registry is modified. Updates are lazily applied when results() is called
     *
     * @tparam TComponents The types of components in the result set
     */
    template<typename... TComponents>
    class View : private ViewBase {
    public:
        using Ptr = std::shared_ptr<View>;

        virtual ~View() = default;

        /**
         * @brief Returns the set of results
         *
         */
        const std::unordered_map<Entity, ComponentSet<TComponents...>>& results();

        /**
         * @brief Returns the beginning of the result set
         *
         */
        typename std::unordered_map<Entity, ComponentSet<TComponents...>>::iterator begin();

        /**
         * @brief Returns the end of the result set
         *
         */
        typename std::unordered_map<Entity, ComponentSet<TComponents...>>::iterator end();

    private:
        std::unordered_map<Entity, ComponentSet<TComponents...>> entities;

        View(Registry& registry);
        void refresh();

        friend class Registry;
    };

    /**
     * @brief Generates and returns a View containing the entities with the given set of components
     *
     * @tparam TComponents The types of components entities must have to be included
     * @return View<TComponents...>::Ptr A View containing the matching entities
     */
    template<typename... TComponents>
    typename View<TComponents...>::Ptr getEntitiesWithComponents();

    /**
     * @brief Returns a set of components for the given entity. Missing components will be nullptr
     *
     * @tparam TComponents The types of components to pull
     * @param e The entity to get the components from
     * @return ComponentSet<TComponents...> The set of components. Some may be nullptr
     */
    template<typename... TComponents>
    ComponentSet<TComponents...> getEntityComponents(Entity e);

private:
    using ComponentStorage = container::Any<32>; // TODO - benchmark this
    using ComponentPool    = container::DynamicObjectPool<ComponentStorage>;

    mutable std::shared_mutex entityMutex;
    std::unordered_set<Entity> entities;
    bl::event::Dispatcher* dispatcher;

    std::unordered_map<Component::IdType, ComponentPool> componentPools;
    std::unordered_map<Component::IdType, std::unordered_set<Entity>> componentEntities;
    std::unordered_map<Entity, std::unordered_map<Component::IdType, ComponentPool::Iterator>>
        entityComponentIterators;

    std::unordered_set<ViewBase*> activeViews;
    std::shared_mutex viewMutex;

    void addView(ViewBase* view);
    void removeView(ViewBase* view);

    void invalidateViews(Component::IdType cid);

    template<typename... TComponents>
    void populateView(std::unordered_map<Entity, ComponentSet<TComponents...>>& results);

    template<typename TComponent, typename... TComponents>
    bool populateComponent(Entity entity, ComponentSet<TComponents...>& set);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
bool Registry::addComponent(Entity entity, const T& component) {
    const Component::IdType& id = Component::getId<T>();
    std::unique_lock lock(entityMutex);

    // entity does not exist
    if (entities.find(entity) == entities.end()) return false;

    // ensure pool exists
    auto poolIter = componentPools.find(id);
    if (poolIter == componentPools.end()) { poolIter = componentPools.try_emplace(id).first; }

    // ensure component link exists
    auto linkIter = componentEntities.find(id);
    if (linkIter == componentEntities.end()) { linkIter = componentEntities.try_emplace(id).first; }

    // ensure entity does not already have component
    auto indexIter = entityComponentIterators.find(entity);
    if (indexIter == entityComponentIterators.end()) {
        indexIter = entityComponentIterators.try_emplace(entity).first;
    }
    if (indexIter->second.find(id) != indexIter->second.end()) return false;

    // add and track component
    auto it = poolIter->second.add(component);
    indexIter->second.emplace(id, it);
    linkIter->second.insert(entity);

    invalidateViews(id);
    return true;
}

template<typename T>
bool Registry::hasComponent(Entity entity) const {
    const Component::IdType& id = Component::getId<T>();
    std::shared_lock lock(entityMutex);

    auto linkIter = componentEntities.find(id);
    if (linkIter == componentEntities.end()) return false;
    return linkIter->second.find(entity) != linkIter->second.end();
}

template<typename T>
T* Registry::getComponent(Entity entity) {
    const Component::IdType& id = Component::getId<T>();
    std::shared_lock lock(entityMutex);

    auto indexIter = entityComponentIterators.find(entity);
    if (indexIter == entityComponentIterators.end()) return nullptr;

    auto poolIndexIter = indexIter->second.find(id);
    if (poolIndexIter == indexIter->second.end()) return nullptr;
    return &poolIndexIter->second->get<T>();
}

template<typename T>
bool Registry::removeComponent(Entity entity) {
    const Component::IdType& id = Component::getId<T>();
    std::unique_lock lock(entityMutex);

    auto indexIter = entityComponentIterators.find(entity);
    if (indexIter == entityComponentIterators.end()) return false;

    auto it = indexIter->second.find(id);
    if (it == indexIter->second.end()) return false;

    auto poolIter = componentPools.find(id);
    if (poolIter == componentPools.end()) return false;

    auto linkIter = componentEntities.find(id);
    linkIter->second.erase(entity);
    poolIter->second.erase(it->second);
    indexIter->second.erase(id);

    invalidateViews(id);
    return true;
}

template<typename... TComponents>
typename Registry::View<TComponents...>::Ptr Registry::getEntitiesWithComponents() {
    typename View<TComponents...>::Ptr view(new View<TComponents...>(*this));
    populateView(view->entities);
    return view;
}

template<typename... TComponents>
void Registry::populateView(std::unordered_map<Entity, ComponentSet<TComponents...>>& results) {
    results.clear();
    const Component::IdType ids[] = {Component::getId<TComponents>()...};
    if (sizeof...(TComponents) == 0) return;

    std::shared_lock lock(entityMutex);

    auto entityIt = componentEntities.find(ids[0]);
    if (entityIt == componentEntities.end()) return;

    for (const Entity e : entityIt->second) {
        ComponentSet<TComponents...> set(e);
        const bool present[] = {populateComponent<TComponents, TComponents...>(e, set)...};
        for (unsigned int i = 0; i < sizeof...(TComponents); ++i) {
            if (!present[i]) goto noAdd;
        }
        results.emplace(e, set);
    noAdd:;
    }
}

template<typename... TComponents>
ComponentSet<TComponents...> Registry::getEntityComponents(Entity e) {
    ComponentSet<TComponents...> set(e);
    std::shared_lock lock(entityMutex);
    const bool present[] = {populateComponent<TComponents, TComponents...>(e, set)...};
    return set;
}

template<typename TComponent, typename... TComponents>
bool Registry::populateComponent(Entity e, ComponentSet<TComponents...>& set) {
    const Component::IdType id = Component::getId<TComponent>();

    auto ecit = entityComponentIterators.find(e);
    if (ecit == entityComponentIterators.end()) return false;

    auto it = ecit->second.find(id);
    if (it == ecit->second.end()) return false;

    set.set(&it->second->get<TComponent>());
    return true;
}

template<typename... Ts>
Registry::ViewBase::ViewBase(Registry& r, DeductionDummy<Ts...> _)
: registry(r)
, dirty(false)
, componentTypes({Component::getId<Ts>()...}) {
    registry.addView(this);
}

template<typename... TComponents>
Registry::View<TComponents...>::View(Registry& r)
: ViewBase(r, ViewBase::DeductionDummy<TComponents...>()) {}

template<typename... TComponents>
const std::unordered_map<Entity, ComponentSet<TComponents...>>&
Registry::View<TComponents...>::results() {
    if (isDirty()) refresh();
    return entities;
}

template<typename... TComponents>
typename std::unordered_map<Entity, ComponentSet<TComponents...>>::iterator
Registry::View<TComponents...>::begin() {
    if (isDirty()) refresh();
    return entities.begin();
}

template<typename... TComponents>
typename std::unordered_map<Entity, ComponentSet<TComponents...>>::iterator
Registry::View<TComponents...>::end() {
    if (isDirty()) refresh();
    return entities.end();
}

template<typename... TComponents>
void Registry::View<TComponents...>::refresh() {
    registry.populateView(entities);
    makeClean();
}

} // namespace entity
} // namespace bl

#endif
