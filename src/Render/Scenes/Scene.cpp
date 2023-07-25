#include <BLIB/Render/Scenes/Scene.hpp>

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
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

void Scene::createAndAddObject(ecs::Entity entity, rcom::DrawableBase& object,
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

void Scene::rebucketObject(rcom::DrawableBase& obj) {
    std::unique_lock lock(batchMutex);
    batchChanges.emplace_back(
        BatchChange{obj.sceneRef.object, obj.pipeline, obj.containsTransparency});
}

void Scene::addGraphTasks(rg::RenderGraph&) {}

std::unique_ptr<cam::Camera> Scene::createDefaultCamera() {
    // TODO - make pure virtual and create 2d + 3d scenes on top of batched scene
    return std::make_unique<cam::Camera2D>(sf::FloatRect{0.f, 0.f, 1920.f, 1080.f});
}

} // namespace rc
} // namespace bl
