#include <BLIB/Render/Scenes/Scene.hpp>

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Render/Config/Constants.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
Scene::Scene(engine::Engine& engine)
: engine(engine)
, renderer(engine.renderer())
, shaderInputStore(engine)
, staticPipelines(cfg::Constants::DefaultSceneObjectCapacity, nullptr)
, dynamicPipelines(cfg::Constants::DefaultSceneObjectCapacity, nullptr)
, syncedResourcesThisFrame(false)
, isClearingQueues(false) {
    queuedBatchChanges.reserve(32);
    queuedAdds.reserve(32);
    queuedRemovals.reserve(32);
}

std::uint32_t Scene::registerObserver(RenderTarget* target) {
    const std::uint32_t index = targetTable.addTarget(target);
#ifdef BLIB_DEBUG
    if (index >= cfg::Limits::MaxSceneObservers) {
        BL_LOG_CRITICAL << "Max observer count for scene reached";
        throw std::runtime_error("Max observer count for scene reached");
    }
#endif
    doRegisterObserver(target, index);
    return index;
}

void Scene::unregisterObserver(std::uint32_t index) {
    RenderTarget* target = targetTable.removeTarget(index);
    if (index < targetTable.nextId()) { doUnregisterObserver(target, index); }
}

void Scene::syncShaderResources() {
    if (!syncedResourcesThisFrame) {
        syncedResourcesThisFrame = true;

        std::unique_lock lock(objectMutex);
        onShaderResourceSync();
        shaderInputStore.updateFromSources();
        shaderInputStore.performTransfers();
    }
}

void Scene::syncObjects() {
    std::unique_lock lock(objectMutex);
    std::unique_lock queueLock(queueMutex);
    isClearingQueues = true;

    // do batch changes
    for (const auto& change : queuedBatchChanges) {
        auto& objectPipelines = change.changed->sceneKey.updateFreq == UpdateSpeed::Static ?
                                    staticPipelines :
                                    dynamicPipelines;
        doBatchChange(change, objectPipelines[change.changed->sceneKey.sceneId]);
        objectPipelines[change.changed->sceneKey.sceneId] = change.newPipeline;
    }
    queuedBatchChanges.clear();

    // remove queued objects
    for (auto* obj : queuedRemovals) { removeQueuedObject(obj); }
    queuedRemovals.clear();

    // add queued objects
    ecs::Transaction<ecs::tx::EntityRead> tx(engine.ecs());
    for (auto& add : queuedAdds) {
        if (engine.ecs().entityExists(add.entity, tx)) { addQueuedObject(add); }
    }
    queuedAdds.clear();

    isClearingQueues = false;

    // copy ECS components into descriptor buffers
    shaderInputStore.updateFromSources();
}

void Scene::createAndAddObject(ecs::Entity entity, rcom::DrawableBase& object,
                               UpdateSpeed updateFreq) {
    std::unique_lock lock(queueMutex);
    queuedAdds.emplace_back(entity, &object, updateFreq);
}

void Scene::removeObject(scene::SceneObject* obj) {
    std::unique_lock lock(queueMutex);

    if (isClearingQueues) { removeQueuedObject(obj); }
    else { queuedRemovals.emplace_back(obj); }
}

void Scene::rebucketObject(rcom::DrawableBase& obj) {
    std::unique_lock lock(queueMutex);
    queuedBatchChanges.emplace_back(
        BatchChange{.changed           = obj.sceneRef.object,
                    .newPipeline       = obj.getCurrentPipeline(),
                    .newTrans          = obj.containsTransparency,
                    .newSpecialization = obj.getPipelineSpecialization()});
}

void Scene::removeQueuedObject(scene::SceneObject* obj) {
    std::unique_lock lock(queueMutex);

    auto& objectPipelines =
        obj->sceneKey.updateFreq == UpdateSpeed::Static ? staticPipelines : dynamicPipelines;
    const auto cachedKey = obj->sceneKey.sceneId;
    mat::MaterialPipeline* pipeline =
        cachedKey < objectPipelines.size() ? objectPipelines[cachedKey] : nullptr;
    if (pipeline != nullptr) {
        doObjectRemoval(obj, pipeline);
        objectPipelines[cachedKey] = nullptr;
    }
}

void Scene::addQueuedObject(ObjectAdd& add) {
    rcom::DrawableBase& object = *add.object;
    scene::SceneObject* sobj   = doAdd(add.entity, object, add.updateFreq);
    if (sobj) {
        sobj->entity           = add.entity;
        object.sceneRef.object = sobj;
        object.sceneRef.scene  = this;
        sobj->component        = &object;

        auto& objectPipelines =
            sobj->sceneKey.updateFreq == UpdateSpeed::Static ? staticPipelines : dynamicPipelines;
        if (sobj->sceneKey.sceneId >= objectPipelines.size()) {
            objectPipelines.resize(sobj->sceneKey.sceneId + 1, nullptr);
        }
        objectPipelines[sobj->sceneKey.sceneId] = object.getCurrentPipeline();
    }
    else { BL_LOG_ERROR << "Failed to add " << add.entity << " to scene " << this; }
}

void Scene::renderScene(scene::SceneRenderContext& ctx) {
    renderOpaqueObjects(ctx);
    renderTransparentObjects(ctx);
}

ds::DescriptorSetInstanceCache* Scene::getDescriptorSetCache(RenderTarget* target) {
    ds::DescriptorSetInstanceCache* cache = target->getDescriptorSetCache(this);
    if (!cache) { BL_LOG_ERROR << "Failed to get descriptor set cache for scene from target"; }
    return cache;
}

} // namespace rc
} // namespace bl
