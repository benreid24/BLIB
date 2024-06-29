#include <BLIB/Render/Scenes/Scene.hpp>

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace
{
constexpr std::uint32_t NoPipeline = std::numeric_limits<std::uint32_t>::max();
}

Scene::Scene(engine::Engine& engine,
             const ds::DescriptorComponentStorageBase::EntityCallback& entityCb)
: renderer(engine.renderer())
, descriptorFactories(renderer.descriptorFactoryCache())
, descriptorSets(descriptorComponents)
, descriptorComponents(engine.ecs(), renderer.vulkanState(), entityCb)
, nextObserverIndex(0)
, staticPipelines(DefaultSceneObjectCapacity, NoPipeline)
, dynamicPipelines(DefaultSceneObjectCapacity, NoPipeline) {
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
    descriptorSets.handleDescriptorSync();
    descriptorComponents.syncDescriptors();
}

void Scene::createAndAddObject(ecs::Entity entity, rcom::DrawableBase& object,
                               UpdateSpeed updateFreq) {
    std::unique_lock lock(objectMutex);

    scene::SceneObject* sobj = doAdd(entity, object, updateFreq);
    if (sobj) {
        object.sceneRef.object = sobj;
        object.sceneRef.scene  = this;

        sobj->hidden     = object.hidden;
        sobj->drawParams = object.drawParams;

        auto& objectPipelines =
            sobj->sceneKey.updateFreq == UpdateSpeed::Static ? staticPipelines : dynamicPipelines;
        if (sobj->sceneKey.sceneId >= objectPipelines.size()) {
            objectPipelines.resize(sobj->sceneKey.sceneId + 1, NoPipeline);
        }
        objectPipelines[sobj->sceneKey.sceneId] = object.pipeline;
    }
    else { BL_LOG_ERROR << "Failed to add " << entity << " to scene " << this; }
}

void Scene::removeObject(scene::SceneObject* obj) {
    std::unique_lock lock(objectMutex);

    auto& objectPipelines =
        obj->sceneKey.updateFreq == UpdateSpeed::Static ? staticPipelines : dynamicPipelines;
    const auto cachedKey = obj->sceneKey.sceneId;
    std::uint32_t pipeline =
        cachedKey < objectPipelines.size() ? objectPipelines[cachedKey] : NoPipeline;
    if (pipeline != NoPipeline) {
        doObjectRemoval(obj, pipeline);
        objectPipelines[cachedKey] = NoPipeline;
    }
}

void Scene::rebucketObject(rcom::DrawableBase& obj) {
    std::unique_lock lock(batchMutex);
    batchChanges.emplace_back(
        BatchChange{obj.sceneRef.object, obj.pipeline, obj.containsTransparency});
}

void Scene::addGraphTasks(rg::RenderGraph&) {}

} // namespace rc
} // namespace bl
