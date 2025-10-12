#include <BLIB/Render/Descriptors/InstanceTable.hpp>

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/RenderTarget.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
InstanceTable::InstanceTable()
: factory(nullptr)
, scene(nullptr) {}

void InstanceTable::init(Scene* s, DescriptorSetFactory* f) {
    scene   = s;
    factory = f;
}

void InstanceTable::addObserver(unsigned int index, RenderTarget& target) {
    if (index >= sets.size()) { sets.resize(index + 1, nullptr); }
    sets[index] = target.getDescriptorSetCache(scene)->getDescriptorSet(factory);
}

bool InstanceTable::allocateObject(ecs::Entity entity, scene::Key key) {
    bool success = true;
    for (auto* set : sets) {
        if (set && !set->allocateObject(entity, key)) { success = false; }
    }
    return success;
}

bool InstanceTable::allocateObject(unsigned int instanceIndex, ecs::Entity entity, scene::Key key) {
    if (instanceIndex >= sets.size() || !sets[instanceIndex]) { return false; }
    return sets[instanceIndex]->allocateObject(entity, key);
}

void InstanceTable::releaseObject(ecs::Entity entity, scene::Key key) {
    for (auto* set : sets) {
        if (set) { set->releaseObject(entity, key); }
    }
}

} // namespace ds
} // namespace rc
} // namespace bl
