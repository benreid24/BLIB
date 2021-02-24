#ifndef BLIB_ENTITIES_REGISTRY_HPP
#define BLIB_ENTITIES_REGISTRY_HPP

#include <BLIB/Containers/Any.hpp>
#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <BLIB/Entities/Component.hpp>
#include <BLIB/Entities/Entity.hpp>

#include <algorithm>
#include <any>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace bl
{
namespace entity
{
class Registry {
public:
    Entity createEntity();

    bool entityExists(Entity entity) const;

    void destroyEntity(Entity entity);

    template<typename TComponent>
    bool addComponent(Entity entity, const TComponent& component);

    template<typename TComponent>
    bool hasComponent(Entity entity) const;

    template<typename TComponent>
    EntityComponent<TComponent> getComponent(Entity entity);

    template<typename TComponent>
    bool removeComponent(Entity entity);

private:
    using ComponentStorage = Any<32>; // TODO - benchmark this
    using ComponentPool    = DynamicObjectPool<std::pair<Entity, ComponentStorage>>;

    // maybe do object pool style but with available as vector, pop and use index from available, or
    // append new id if available is empty. Entity is sequential id, reusable
    std::unordered_set<Entity> entities;

    std::unordered_map<Component::IdType, ComponentPool> componentPools;
    std::unordered_map<Entity, std::unordered_map<Component::IdType, ComponentPool::Iterator>>
        entityComponentIterators;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
bool Registry::addComponent(Entity entity, const T& component) {
    const Component::IdType& id = Component::getId<T>();

    // entity does not exist
    if (entities.find(entity) == entities.end()) return false;

    // ensure pool exists
    auto poolIter = componentPools.find(id);
    if (poolIter == componentPools.end()) { poolIter = componentPools.try_emplace(id).first; }

    // ensure entity does not already have component
    auto indexIter = entityComponentIterators.find(entity);
    if (indexIter == entityComponentIterators.end()) {
        indexIter = entityComponentIterators.try_emplace(entity).first;
    }
    if (indexIter->second.find(id) != indexIter->second.end()) return false;

    // add and track component
    auto it = poolIter->second.add(std::make_pair(entity, ComponentStorage(component)));
    indexIter->second.emplace(id, it);

    return true;
}

template<typename T>
bool Registry::hasComponent(Entity entity) const {
    const Component::IdType& id = Component::getId<T>();

    auto indexIter = entityComponentIterators.find(entity);
    if (indexIter == entityComponentIterators.end()) return false;
    return indexIter->second.find(id) == indexIter->second.end();
}

template<typename T>
EntityComponent<T> Registry::getComponent(Entity entity) {
    const Component::IdType& id = Component::getId<T>();

    auto indexIter = entityComponentIterators.find(entity);
    if (indexIter == entityComponentIterators.end()) return {entity, nullptr};

    auto poolIndexIter = indexIter->second.find(id);
    if (poolIndexIter == indexIter->second.end()) return {entity, nullptr};
    return {entity, &poolIndexIter->second->second.get<T>()};
}

template<typename T>
bool Registry::removeComponent(Entity entity) {
    const Component::IdType& id = Component::getId<T>();

    auto indexIter = entityComponentIterators.find(entity);
    if (indexIter == entityComponentIterators.end()) return false;

    auto it = indexIter->second.find(id);
    if (it == indexIter->second.end()) return false;

    auto poolIter = componentPools.find(id);
    if (poolIter == componentPools.end()) return false;

    poolIter->second.erase(it->second);
    return true;
}

} // namespace entity
} // namespace bl

#endif
