#ifndef BLIB_ENTITIES_REGISTRY_HPP
#define BLIB_ENTITIES_REGISTRY_HPP

#include <BLIB/Containers/Any.hpp>
#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <BLIB/Entities/Component.hpp>
#include <BLIB/Entities/ComponentSet.hpp>
#include <BLIB/Entities/Entity.hpp>

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
class Registry {
    class ViewBase {
    public:
        ViewBase(Registry& registry);
        virtual ~ViewBase();

        void makeDirty(Component::IdType cid);

    protected:
        std::unordered_set<Component::IdType> componentTypes;

        bool isDirty() const;
        void makeClean();

    private:
        Registry& registry;
        bool dirty;

        friend class Registry;
    };

public:
    using EntityIterator      = std::unordered_set<Entity>::iterator;
    using ConstEntityIterator = std::unordered_set<Entity>::const_iterator;

    Entity createEntity();

    bool entityExists(Entity entity) const;

    void destroyEntity(Entity entity);

    EntityIterator begin();

    ConstEntityIterator begin() const;

    EntityIterator end();

    ConstEntityIterator end() const;

    template<typename TComponent>
    bool addComponent(Entity entity, const TComponent& component);

    template<typename TComponent>
    bool hasComponent(Entity entity) const;

    template<typename TComponent>
    TComponent* getComponent(Entity entity);

    template<typename TComponent>
    bool removeComponent(Entity entity);

    template<typename... TComponents>
    class View : private ViewBase {
    public:
        using Ptr = std::shared_ptr<View>;

        virtual ~View() = default;

        const std::vector<ComponentSet<TComponents...>>& results();

    private:
        std::vector<ComponentSet<TComponents...>> entities;

        View(Registry& registry);
        void refresh();

        friend class Registry;
    };

    template<typename... TComponents>
    typename View<TComponents...>::Ptr getEntitiesWithComponents();

    template<typename... TComponents>
    ComponentSet<TComponents...> getEntityComponents(Entity e);

private:
    using ComponentStorage = Any<32>; // TODO - benchmark this
    using ComponentPool    = DynamicObjectPool<ComponentStorage>;

    mutable std::shared_mutex entityMutex;
    std::unordered_set<Entity> entities;

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
    void populateView(std::vector<ComponentSet<TComponents...>>& results);

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
    auto it = poolIter->second.add(std::make_pair(entity, ComponentStorage(component)));
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
void Registry::populateView(std::vector<ComponentSet<TComponents...>>& results) {
    results.clear();
    const Component::IdType ids[] = {Component::getId<TComponents>()...};
    if (sizeof...(TComponents) == 0) return;

    std::shared_lock lock(entityMutex);

    auto entityIt = componentEntities.find(ids[0]);
    if (entityIt == componentEntities.end()) return;

    results.reserve(entityIt->second.size());
    for (const Entity e : entityIt->second) {
        ComponentSet<TComponents...> set(e);
        const bool present[] = {populateComponent<TComponents, TComponents...>(e, set)...};
        for (unsigned int i = 0; i < sizeof...(TComponents); ++i) {
            if (!present[i]) goto noAdd;
        }
        results.push_back(set);
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

template<typename... TComponents>
const std::vector<ComponentSet<TComponents...>>& Registry::View<TComponents...>::results() {
    if (isDirty()) refresh();
    return entities;
}

template<typename... TComponents>
void Registry::View<TComponents...>::refresh() {
    registry.populateView(entities);
    makeClean();
}

} // namespace entity
} // namespace bl

#endif
