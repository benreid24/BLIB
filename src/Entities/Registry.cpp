#include <BLIB/Entities.hpp>

namespace bl
{
namespace entity
{
Entity Registry::createEntity() {
    const Entity e = IdGenerator::makeNew();
    entities.insert(e);
    return e;
}

bool Registry::entityExists(Entity e) const { return entities.find(e) != entities.end(); }

void Registry::destroyEntity(Entity e) {
    // remove components
    auto it = entityComponentIterators.find(e);
    if (it != entityComponentIterators.end()) {
        for (const auto& component : it->second) {
            auto pool = componentPools.find(component.first);
            if (pool != componentPools.end()) {
                pool->second.erase(component.second); // erase from storage
            }
        }
        entityComponentIterators.erase(it);
    }
    entities.erase(e);
}

struct trash {
    static constexpr Component::IdType ComponentId = 1;

    int data[4];
    bool isTrash;
};

void temp() {
    Registry registry;
    Entity e = registry.createEntity();
    registry.addComponent(e, trash());
    registry.hasComponent<trash>(e);
    registry.getComponent<trash>(e);
    registry.removeComponent<trash>(e);
}

} // namespace entity
} // namespace bl
