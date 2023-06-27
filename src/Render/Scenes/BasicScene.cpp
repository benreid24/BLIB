#include <BLIB/Render/Scenes/BasicScene.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace scene
{
BasicScene::BasicScene(Renderer& r, std::uint32_t maxStatic, std::uint32_t maxDynamic)
: Scene(r, maxStatic, maxDynamic)
, objects(maxStatic + maxDynamic)
, opaqueObjects(r, maxStatic + maxDynamic, descriptorSets)
, transparentObjects(r, maxStatic + maxDynamic, descriptorSets) {}

scene::SceneObject* BasicScene::doAdd(ecs::Entity entity, std::uint32_t sceneId,
                                      UpdateSpeed updateFreq, std::uint32_t pipeline) {
    scene::SceneObject* object = &objects[sceneId];
    object->sceneId            = sceneId;

    // TODO - transparency from object
    const bool isTransparent = false;
    auto& batch              = isTransparent ? transparentObjects : opaqueObjects;
    if (!batch.addObject(object, pipeline, entity, updateFreq)) {
        BL_LOG_ERROR << "Failed to add " << entity << " to scene " << this;
        return nullptr;
    }

    return object;
}

void BasicScene::doRemove(ecs::Entity ent, scene::SceneObject* obj, std::uint32_t pipeline) {
    // TODO - transparency from object
    const bool isTransparent = false;
    auto& batch              = isTransparent ? transparentObjects : opaqueObjects;
    batch.removeObject(obj, pipeline, ent);
}

void BasicScene::renderScene(scene::SceneRenderContext& ctx) {
    opaqueObjects.recordRenderCommands(ctx);
    transparentObjects.recordRenderCommands(ctx);
}

} // namespace scene
} // namespace gfx
} // namespace bl
