#include <BLIB/Render/Descriptors/InstanceTable.hpp>

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/RenderTarget.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/TargetTable.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
InstanceTable::InstanceTable()
: scene(nullptr)
, checkedBindless(false)
, bindless(false)
, perObjStart(0) {}

void InstanceTable::init(Scene* s, const vk::PipelineLayout& layout) {
    scene = s;
    for (unsigned int i = 0; i < layout.getDescriptorSetCount(); ++i) {
        factories.emplace_back(layout.getDescriptorSetFactory(i));
    }
}

void InstanceTable::reinit(const vk::PipelineLayout& layout, scene::TargetTable& observers,
                           const scene::SceneObject& obj) {
    // capture prior state and reset
    const auto prevFactories = factories;
    const auto prevSets      = sets;
    sets.clear();
    factories.clear();

    // grab new factories
    init(scene, layout);

    // populate new sets per observer
    checkedBindless = false;
    for (unsigned int i = 0; i < observers.nextId(); ++i) {
        addObserver(i, *observers.getTarget(i));
    }

    // remove from sets no longer in
    for (unsigned int i = 0; i < prevFactories.size(); ++i) {
        bool found = false;
        for (unsigned int j = 0; j < factories.size(); ++j) {
            if (prevFactories[i] == factories[j]) {
                found = true;
                break;
            }
        }
        if (!found) {
            for (auto& layoutSet : prevSets) {
                if (layoutSet[i]) { layoutSet[i]->releaseObject(obj.entity, obj.sceneKey); }
            }
        }
    }

    // add to sets that was not in before
    for (unsigned int i = 0; i < factories.size(); ++i) {
        bool found = false;
        for (unsigned int j = 0; j < prevFactories.size(); ++j) {
            if (factories[i] == prevFactories[j]) {
                found = true;
                break;
            }
        }
        if (!found) {
            for (auto& layoutSet : sets) {
                if (layoutSet[i]) { layoutSet[i]->allocateObject(obj.entity, obj.sceneKey); }
            }
        }
    }
}

void InstanceTable::addObservers(const scene::TargetTable& observers) {
    for (unsigned int i = 0; i < observers.nextId(); ++i) {
        RenderTarget* t = observers.getTarget(i);
        if (t) { addObserver(i, *t); }
    }
}

void InstanceTable::addObserver(unsigned int index, RenderTarget& target) {
    if (index >= sets.size()) { sets.resize(index + 1, {}); }
    auto& instances = sets[index];
    for (unsigned int i = 0; i < factories.size(); ++i) {
        instances[i] = target.getDescriptorSetCache(scene)->getDescriptorSet(factories[i]);
    }
    checkBindless();
}

void InstanceTable::removeObserver(unsigned int index) {
    if (index < sets.size()) { sets[index].fill(nullptr); }
}

void InstanceTable::checkBindless() {
    if (!checkedBindless && !sets.empty()) {
        checkedBindless = true;
        for (std::uint8_t i = 0; i < factories.size(); ++i) {
            if (!sets.back()[i]->isBindless()) {
                bindless    = false;
                perObjStart = i;
                break;
            }
        }
    }
}

bool InstanceTable::allocateObject(ecs::Entity entity, scene::Key key) {
    bool success = true;
    for (unsigned int i = 0; i < sets.size(); ++i) {
        success = success && allocateObject(i, entity, key);
    }
    return success;
}

bool InstanceTable::allocateObject(unsigned int instanceIndex, ecs::Entity entity, scene::Key key) {
    auto& layoutSet = sets[instanceIndex];
    bool success    = true;
    for (unsigned int i = 0; i < factories.size(); ++i) {
        if (layoutSet[i] && !layoutSet[i]->allocateObject(entity, key)) { success = false; }
    }
    return success;
}

void InstanceTable::releaseObject(ecs::Entity entity, scene::Key key) {
    for (auto& layoutSet : sets) {
        for (unsigned int i = 0; i < factories.size(); ++i) {
            layoutSet[i]->releaseObject(entity, key);
        }
    }
}

} // namespace ds
} // namespace rc
} // namespace bl
