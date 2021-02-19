#ifndef BLIB_ENTITIES_REGISTRY_HPP
#define BLIB_ENTITIES_REGISTRY_HPP

#include <BLIB/Entities/Component.hpp>
#include <BLIB/Entities/Entity.hpp>

#include <algorithm>
#include <any>
#include <list>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace entity
{
class Registry {
public:
    template<typename TComponent>
    bool addComponent(Entity entity, const TComponent& component);

private:
    // TODO- views
    std::vector<Entity> entities; // unordered set? main data access is components
    std::unordered_map<Component::IdType, std::vector<std::pair<Entity, std::any>>>
        componentPools;
    std::unordered_map<Entity, std::unordered_map<Component::IdType, Component::IdType>>
        entityComponentIndices;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
bool Registry::addComponent(Entity entity, const T& component) {
    if (std::find(entities.begin(), entities.end(), entity) == entities.end()) return false;

    //
}

} // namespace entity
} // namespace bl

#endif
