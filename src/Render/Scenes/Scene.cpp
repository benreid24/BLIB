#include <BLIB/Render/Scenes/Scene.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
Scene::Scene(Renderer& r, std::uint32_t maxStatic, std::uint32_t maxDynamic)
: maxStatic(maxStatic)
, renderer(r)
, descriptorFactories(r.descriptorFactoryCache())
, staticIds(maxStatic)
, dynamicIds(maxDynamic)
, entityMap(maxStatic + maxDynamic, ecs::InvalidEntity)
, objectPipelines(maxStatic + maxDynamic)
, descriptorSets(maxStatic, maxDynamic)
, nextObserverIndex(0) {
    batchChanges.reserve(32);
}

Scene::~Scene() {
    for (std::uint32_t i = 0; i < entityMap.size(); ++i) {
        if (entityMap[i] != ecs::InvalidEntity) {
            descriptorSets.unlinkSceneObject(i, entityMap[i]);
        }
    }
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
            doBatchChange(change, objectPipelines[change.changed->sceneId]);
            objectPipelines[change.changed->sceneId] = change.newPipeline;
        }
        batchChanges.clear();
    }
    descriptorSets.handleDescriptorSync();
}

void Scene::createAndAddObject(ecs::Entity entity, com::DrawableBase& object,
                               UpdateSpeed updateFreq) {
    auto& ids                  = updateFreq == UpdateSpeed::Dynamic ? dynamicIds : staticIds;
    const std::uint32_t offset = updateFreq == UpdateSpeed::Dynamic ? maxStatic : 0;
    if (!ids.available()) {
        BL_LOG_ERROR << "BasicScene " << this << " out of static or dynamic object space";
        return;
    }
    const std::uint32_t i    = ids.allocate() + offset;
    scene::SceneObject* sobj = doAdd(entity, object, i, updateFreq);
    if (sobj) {
        object.sceneRef.object = sobj;
        object.sceneRef.scene  = this;

        sobj->hidden     = false;
        sobj->sceneId    = i;
        sobj->drawParams = object.drawParams;

        entityMap[i]       = entity;
        objectPipelines[i] = object.pipeline;
    }
    else {
        ids.release(i - offset);
        BL_LOG_ERROR << "Failed to add " << entity << " to scene " << this;
    }
}

void Scene::removeObject(scene::SceneObject* obj) {
    const std::size_t i    = obj->sceneId;
    const ecs::Entity ent  = entityMap[i];
    std::uint32_t pipeline = objectPipelines[i];

    entityMap[i] = ecs::InvalidEntity;
    if (i < maxStatic) { staticIds.release(i); }
    else { dynamicIds.release(i - maxStatic); }

    doRemove(ent, obj, pipeline);
}

void Scene::rebucketObject(com::DrawableBase& obj) {
    std::unique_lock lock(batchMutex);
    batchChanges.emplace_back(
        BatchChange{obj.sceneRef.object, obj.pipeline, obj.containsTransparency});
}

} // namespace gfx
} // namespace bl
