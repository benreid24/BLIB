#include <BLIB/Render/Scenes/Scene.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
Scene::Scene(engine::Engine& engine,
             const ds::DescriptorComponentStorageBase::EntityCallback& entityCb)
: renderer(engine.renderer())
, descriptorFactories(renderer.descriptorFactoryCache())
, descriptorSets(descriptorComponents)
, descriptorComponents(engine.ecs(), renderer.vulkanState(), entityCb)
, nextObserverIndex(0)
, staticPipelines(DefaultSceneObjectCapacity)
, dynamicPipelines(DefaultSceneObjectCapacity) {
    batchChanges.reserve(32);
}

std::uint32_t Scene::registerObserver() {
#ifdef BLIB_DEBUG
    if (nextObserverIndex >= Config::MaxSceneObservers) {
        BL_LOG_CRITICAL << "Max observer count for scene reached";
        throw std::runtime_error("Max observer count for scene reached");
    }
#endif
    return nextObserverIndex++;
}

void Scene::updateObserverCamera(std::uint32_t observerIndex, const glm::mat4& projView) {
    descriptorSets.updateObserverCamera(observerIndex, projView);
}

void Scene::handleDescriptorSync() {
    if (!batchChanges.empty()) {
        std::unique_lock lock(batchMutex);
        for (const auto& change : batchChanges) {
            auto& objectPipelines = change.changed->sceneKey.updateFreq == UpdateSpeed::Static ?
                                        staticPipelines :
                                        dynamicPipelines;
            doBatchChange(change, objectPipelines[change.changed->sceneKey.sceneId]);
            objectPipelines[change.changed->sceneKey.sceneId] = change.newPipeline;
        }
        batchChanges.clear();
    }
    descriptorComponents.syncDescriptors();
    descriptorSets.handleDescriptorSync();
}

void Scene::createAndAddObject(ecs::Entity entity, com::DrawableBase& object,
                               UpdateSpeed updateFreq) {
    scene::SceneObject* sobj = doAdd(entity, object, updateFreq);
    if (sobj) {
        object.sceneRef.object = sobj;
        object.sceneRef.scene  = this;

        sobj->hidden     = false;
        sobj->drawParams = object.drawParams;

        auto& objectPipelines =
            sobj->sceneKey.updateFreq == UpdateSpeed::Static ? staticPipelines : dynamicPipelines;
        objectPipelines[sobj->sceneKey.sceneId] = object.pipeline;
    }
    else { BL_LOG_ERROR << "Failed to add " << entity << " to scene " << this; }
}

void Scene::removeObject(scene::SceneObject* obj) {
    auto& objectPipelines =
        obj->sceneKey.updateFreq == UpdateSpeed::Static ? staticPipelines : dynamicPipelines;
    std::uint32_t pipeline = objectPipelines[obj->sceneKey.sceneId];
    doRemove(obj, pipeline);
}

void Scene::rebucketObject(com::DrawableBase& obj) {
    std::unique_lock lock(batchMutex);
    batchChanges.emplace_back(
        BatchChange{obj.sceneRef.object, obj.pipeline, obj.containsTransparency});
}

} // namespace gfx
} // namespace bl
