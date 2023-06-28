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
, transparentObjects(r, maxStatic + maxDynamic, descriptorSets)
, transCache(maxStatic + maxDynamic, false) {}

scene::SceneObject* BasicScene::doAdd(ecs::Entity entity, com::DrawableBase& obj,
                                      std::uint32_t sceneId, UpdateSpeed updateFreq) {
    scene::SceneObject* object = &objects[sceneId];
    object->sceneId            = sceneId;
    transCache[sceneId]        = obj.containsTransparency;

    auto& batch = obj.containsTransparency ? transparentObjects : opaqueObjects;
    if (!batch.addObject(object, obj.pipeline, entity, updateFreq)) {
        BL_LOG_ERROR << "Failed to add " << entity << " to scene " << this;
        return nullptr;
    }

    return object;
}

void BasicScene::doRemove(ecs::Entity ent, scene::SceneObject* obj, std::uint32_t pipeline) {
    auto& batch = transCache[obj->sceneId] ? transparentObjects : opaqueObjects;
    batch.removeObject(obj, pipeline, ent);
}

void BasicScene::doBatchChange(const BatchChange& change, std::uint32_t ogPipeline) {
    // TODO - impl after refactor to batch by layout
}

void BasicScene::renderScene(scene::SceneRenderContext& ctx) {
    opaqueObjects.recordRenderCommands(ctx);
    transparentObjects.recordRenderCommands(ctx);
}

} // namespace scene
} // namespace gfx
} // namespace bl
