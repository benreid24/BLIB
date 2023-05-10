#include <BLIB/Render/Scenes/Scene.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Descriptors/Builtin/CommonSceneDescriptorSetFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/CommonSceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
Scene::Scene(Renderer& r, std::uint32_t maxStatic, std::uint32_t maxDynamic)
: maxStatic(maxStatic)
, renderer(r)
, descriptorFactories(r.descriptorFactoryCache())
, objects(maxStatic + maxDynamic)
, staticIds(maxStatic)
, dynamicIds(maxDynamic)
, entityMap(maxStatic + maxDynamic, ecs::InvalidEntity)
, objectPipelines(maxStatic + maxDynamic)
, descriptorSets(maxStatic, maxDynamic)
, opaqueObjects(r, maxStatic + maxDynamic, descriptorSets)
, transparentObjects(r, maxStatic + maxDynamic, descriptorSets) {
    useSceneDescriptorSet<ds::CommonSceneDescriptorSetFactory,
                          ds::CommonSceneDescriptorSetInstance>();
}

Scene::~Scene() {
    for (std::uint32_t i = 0; i < objects.size(); ++i) {
        if (entityMap[i] != ecs::InvalidEntity) {
            descriptorSets.unlinkSceneObject(i, entityMap[i]);
        }
    }
}

std::uint32_t Scene::registerObserver() { return sceneDescriptors->registerObserver(); }

void Scene::updateObserverCamera(std::uint32_t observerIndex, const glm::mat4& projView) {
    sceneDescriptors->updateObserverCamera(observerIndex, projView);
}

SceneObject* Scene::createAndAddObject(ecs::Entity entity, const DrawParameters& drawParams,
                                       SceneObject::UpdateSpeed updateFreq,
                                       const scene::StagePipelines& pipelines) {
    auto& ids = updateFreq == SceneObject::UpdateSpeed::Dynamic ? dynamicIds : staticIds;
    const std::uint32_t offset = updateFreq == SceneObject::UpdateSpeed::Dynamic ? maxStatic : 0;
    if (!ids.available()) {
        BL_LOG_ERROR << "Scene " << this << " out of static or dynamic object space";
        return nullptr;
    }
    const std::uint32_t i = ids.allocate() + offset;
    SceneObject* object   = &objects[i];
    object->hidden        = false;
    object->sceneId       = i;
    object->drawParams    = drawParams;

    entityMap[i]       = entity;
    objectPipelines[i] = pipelines;

    if (pipelines[Config::SceneObjectStage::OpaquePass] != Config::PipelineIds::None) {
        opaqueObjects.addObject(
            object, pipelines[Config::SceneObjectStage::OpaquePass], entity, updateFreq);
    }
    if (pipelines[Config::SceneObjectStage::TransparentPass] != Config::PipelineIds::None) {
        transparentObjects.addObject(
            object, pipelines[Config::SceneObjectStage::TransparentPass], entity, updateFreq);
    }

    // TODO - account for object add failure and remove
    return object;
}

void Scene::removeObject(SceneObject* obj) {
    const std::size_t i                    = obj - objects.data();
    const ecs::Entity ent                  = entityMap[i];
    const scene::StagePipelines& pipelines = objectPipelines[i];

    entityMap[i] = ecs::InvalidEntity;
    if (i < maxStatic) { staticIds.release(i); }
    else { dynamicIds.release(i - maxStatic); }

    if (pipelines[Config::SceneObjectStage::OpaquePass] != Config::PipelineIds::None) {
        opaqueObjects.removeObject(obj, pipelines[Config::SceneObjectStage::OpaquePass], ent);
    }
    if (pipelines[Config::SceneObjectStage::TransparentPass] != Config::PipelineIds::None) {
        transparentObjects.removeObject(
            obj, pipelines[Config::SceneObjectStage::TransparentPass], ent);
    }
}

void Scene::removeObject(ecs::Entity ent) {
    for (unsigned int i = 0; i < entityMap.size(); ++i) {
        if (entityMap[i] == ent) {
            removeObject(&objects[i]);
            return;
        }
    }
}

void Scene::renderScene(SceneRenderContext& ctx) {
    // TODO - support additional steps here, like recording to off-screen textures
    opaqueObjects.recordRenderCommands(ctx);
    transparentObjects.recordRenderCommands(ctx);
}

void Scene::handleDescriptorSync() { descriptorSets.handleDescriptorSync(); }

} // namespace render
} // namespace bl
