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
, nextObserverIndex(0) {}

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

void Scene::handleDescriptorSync() { descriptorSets.handleDescriptorSync(); }

scene::SceneObject* Scene::createAndAddObject(ecs::Entity entity,
                                              const prim::DrawParameters& drawParams,
                                              UpdateSpeed updateFreq, std::uint32_t pipeline) {
    auto& ids                  = updateFreq == UpdateSpeed::Dynamic ? dynamicIds : staticIds;
    const std::uint32_t offset = updateFreq == UpdateSpeed::Dynamic ? maxStatic : 0;
    if (!ids.available()) {
        BL_LOG_ERROR << "BasicScene " << this << " out of static or dynamic object space";
        return nullptr;
    }
    const std::uint32_t i      = ids.allocate() + offset;
    scene::SceneObject* object = doAdd(entity, i, updateFreq, pipeline);
    if (object) {
        object->hidden     = false;
        object->sceneId    = i;
        object->drawParams = drawParams;

        entityMap[i]       = entity;
        objectPipelines[i] = pipeline;
    }
    else { ids.release(i - offset); }

    return object;
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

} // namespace gfx
} // namespace bl
