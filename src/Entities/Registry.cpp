#include <BLIB/Entities.hpp>

namespace bl
{
namespace entity
{
Entity Registry::createEntity() {
    const Entity e = IdGenerator::makeNew();
    std::unique_lock lock(entityMutex);
    entities.insert(e);
    return e;
}

bool Registry::entityExists(Entity e) const { return entities.find(e) != entities.end(); }

void Registry::destroyEntity(Entity e) {
    std::unique_lock lock(entityMutex);

    auto it = entityComponentIterators.find(e);
    if (it != entityComponentIterators.end()) {
        for (const auto& component : it->second) {
            invalidateViews(component.first);
            auto pool = componentPools.find(component.first);
            if (pool != componentPools.end()) {
                pool->second.erase(component.second); // erase from storage
            }
        }
        entityComponentIterators.erase(it);
    }
    entities.erase(e);
}

Registry::EntityIterator Registry::begin() { return entities.begin(); }

Registry::ConstEntityIterator Registry::begin() const { return entities.begin(); }

Registry::EntityIterator Registry::end() { return entities.end(); }

Registry::ConstEntityIterator Registry::end() const { return entities.end(); }

void Registry::addView(ViewBase* v) {
    std::unique_lock lock(viewMutex);
    activeViews.insert(v);
}

void Registry::removeView(ViewBase* v) {
    std::unique_lock lock(viewMutex);
    activeViews.erase(v);
}

void Registry::invalidateViews(Component::IdType cid) {
    std::shared_lock lock(viewMutex);
    for (ViewBase* view : activeViews) { view->makeDirty(cid); }
}

Registry::ViewBase::ViewBase(Registry& r)
: registry(r)
, dirty(false) {
    registry.addView(this);
}

Registry::ViewBase::~ViewBase() { registry.removeView(this); }

void Registry::ViewBase::makeDirty(Component::IdType cid) {
    if (componentTypes.find(cid) != componentTypes.end()) { dirty = true; }
}

bool Registry::ViewBase::isDirty() const { return dirty; }

void Registry::ViewBase::makeClean() { dirty = false; }

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
    Registry::View<trash>::Ptr view = registry.getEntitiesWithComponents<trash>();
    view->results();
    ComponentSet<trash> set = registry.getEntityComponents<trash>(e);
}

} // namespace entity
} // namespace bl
