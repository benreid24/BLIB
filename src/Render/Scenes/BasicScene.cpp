#include <BLIB/Render/Scenes/BasicScene.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace scene
{
BasicScene::BasicScene(Renderer& r, std::uint32_t maxStatic, std::uint32_t maxDynamic)
: Scene(r, maxStatic, maxDynamic)
, objects(maxStatic + maxDynamic)
, opaqueObjects(r, maxStatic + maxDynamic, descriptorSets)
, transparentObjects(r, maxStatic + maxDynamic, descriptorSets) {}

scene::SceneObject* BasicScene::doAdd(ecs::Entity entity, std::uint32_t sceneId,
                                      UpdateSpeed updateFreq,
                                      const scene::StagePipelines& pipelines) {
    scene::SceneObject* object = &objects[sceneId];
    object->sceneId            = sceneId;

    if (pipelines[Config::SceneObjectStage::OpaquePass] != Config::PipelineIds::None) {
        if (!opaqueObjects.addObject(
                object, pipelines[Config::SceneObjectStage::OpaquePass], entity, updateFreq)) {
            BL_LOG_ERROR << "Failed to add " << entity << " to scene " << this;
            return nullptr;
        }
    }
    if (pipelines[Config::SceneObjectStage::TransparentPass] != Config::PipelineIds::None) {
        if (!transparentObjects.addObject(
                object, pipelines[Config::SceneObjectStage::TransparentPass], entity, updateFreq)) {
            BL_LOG_ERROR << "Failed to add " << entity << " to scene " << this;
            opaqueObjects.removeObject(
                object, pipelines[Config::SceneObjectStage::OpaquePass], entity);
            return nullptr;
        }
    }

    return object;
}

void BasicScene::doRemove(ecs::Entity ent, scene::SceneObject* obj,
                          const scene::StagePipelines& pipelines) {
    if (pipelines[Config::SceneObjectStage::OpaquePass] != Config::PipelineIds::None) {
        opaqueObjects.removeObject(obj, pipelines[Config::SceneObjectStage::OpaquePass], ent);
    }
    if (pipelines[Config::SceneObjectStage::TransparentPass] != Config::PipelineIds::None) {
        transparentObjects.removeObject(
            obj, pipelines[Config::SceneObjectStage::TransparentPass], ent);
    }
}

void BasicScene::renderScene(scene::SceneRenderContext& ctx) {
    opaqueObjects.recordRenderCommands(ctx);
    transparentObjects.recordRenderCommands(ctx);
}

} // namespace scene
} // namespace render
} // namespace bl
