#include <BLIB/Render/Scenes/CodeScene.hpp>

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene2DFactory.hpp>
#include <BLIB/Render/Events/SceneObjectRemoved.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
namespace
{
VkRect2D makeScissor(const VkViewport& vp) {
    VkRect2D scissor{};
    scissor.offset.x      = vp.x;
    scissor.offset.y      = vp.y;
    scissor.extent.width  = vp.width;
    scissor.extent.height = vp.height;
    return scissor;
}

VkViewport makeViewport(const VkRect2D& s) {
    VkViewport vp{};
    vp.x        = s.offset.x;
    vp.y        = s.offset.y;
    vp.width    = s.extent.width;
    vp.height   = s.extent.height;
    vp.minDepth = 0.f;
    vp.maxDepth = 1.f;
    return vp;
}

} // namespace

CodeScene::CodeScene(engine::Engine& engine, RenderCallback&& renderCallback)
: Scene(engine, objects.makeEntityCallback())
, renderCallback(renderCallback)
, lighting(static_cast<ds::Scene2DInstance*>(descriptorSets.getDescriptorSet(
      descriptorFactories.getOrCreateFactory<ds::Scene2DFactory>()))) {}

CodeScene::~CodeScene() { objects.unlinkAll(descriptorSets); }

void CodeScene::renderScene(scene::SceneRenderContext& context) {
    RenderContext ctx(context);
    renderCallback(ctx);
}

void CodeScene::doObjectRemoval(SceneObject* object, std::uint32_t pipeline) {
    CodeSceneObject* obj     = static_cast<CodeSceneObject*>(object);
    const ecs::Entity entity = objects.getObjectEntity(obj->sceneKey);

    for (unsigned int i = 0; i < obj->descriptorCount; ++i) {
        obj->descriptors[i]->releaseObject(entity, object->sceneKey);
    }
    objects.release(obj->sceneKey);
    bl::event::Dispatcher::dispatch<rc::event::SceneObjectRemoved>({this, entity});
}

SceneObject* CodeScene::doAdd(ecs::Entity entity, rcom::DrawableBase& object,
                              UpdateSpeed updateFreq) {
    CodeSceneObject& obj = *objects.allocate(updateFreq, entity).newObject;

    obj.pipeline = &renderer.pipelineCache().getPipeline(object.pipeline);
    obj.descriptorCount =
        obj.pipeline->pipelineLayout().initDescriptorSets(descriptorSets, obj.descriptors.data());
    obj.perObjStart = obj.descriptorCount;
    for (unsigned int i = 0; i < obj.descriptorCount; ++i) {
        obj.descriptors[i]->allocateObject(entity, obj.sceneKey);
        if (!obj.descriptors[i]->isBindless()) {
            obj.perObjStart = std::min(obj.perObjStart, static_cast<std::uint8_t>(i));
        }
    }

    return &obj;
}

std::unique_ptr<cam::Camera> CodeScene::createDefaultCamera() {
    return std::make_unique<cam::Camera2D>(sf::FloatRect(0.f,
                                                         0.f,
                                                         renderer.getObserver().getRegionSize().x,
                                                         renderer.getObserver().getRegionSize().y));
}

void CodeScene::setDefaultNearAndFarPlanes(cam::Camera& camera) const {
    camera.setNearAndFarPlanes(0.f, -1000.f);
}

void CodeScene::doBatchChange(const BatchChange& change, std::uint32_t ogPipeline) {
    if (ogPipeline != change.newPipeline) {
        CodeSceneObject& object  = *static_cast<CodeSceneObject*>(change.changed);
        object.pipeline          = &renderer.pipelineCache().getPipeline(change.newPipeline);
        const ecs::Entity entity = objects.getObjectEntity(object.sceneKey);
        object.descriptorCount =
            object.pipeline->pipelineLayout().updateDescriptorSets(descriptorSets,
                                                                   object.descriptors.data(),
                                                                   object.descriptorCount,
                                                                   entity,
                                                                   object.sceneKey.sceneId,
                                                                   object.sceneKey.updateFreq);
        object.perObjStart = object.descriptorCount;
        for (unsigned int i = 0; i < object.descriptorCount; ++i) {
            if (!object.descriptors[i]->isBindless()) {
                object.perObjStart = i;
                break;
            }
        }
    }
}

CodeScene::RenderContext::RenderContext(SceneRenderContext& ctx)
: renderContext(ctx) {}

void CodeScene::RenderContext::renderObject(rcom::DrawableBase& object) {
    CodeSceneObject* obj = static_cast<CodeSceneObject*>(object.sceneRef.object);

#ifdef BLIB_DEBUG
    if (!obj) { throw std::runtime_error("Cannot render object with nullptr scene ref"); }
#endif

    if (!obj->hidden) {
        renderContext.bindPipeline(*obj->pipeline);
        renderContext.bindDescriptors(obj->pipeline->pipelineLayout().rawLayout(),
                                      obj->sceneKey.updateFreq,
                                      obj->descriptors.data(),
                                      obj->descriptorCount);
        for (std::uint8_t i = obj->perObjStart; i < obj->descriptorCount; ++i) {
            obj->descriptors[i]->bindForObject(
                renderContext, obj->pipeline->pipelineLayout().rawLayout(), i, obj->sceneKey);
        }
        renderContext.renderObject(*obj);
    }
}

void CodeScene::RenderContext::setViewport(const VkViewport& vp, bool setScissor) {
    vkCmdSetViewport(renderContext.getCommandBuffer(), 0, 1, &vp);
    if (setScissor) {
        const VkRect2D scissor = makeScissor(vp);
        vkCmdSetScissor(renderContext.getCommandBuffer(), 0, 1, &scissor);
    }
}

void CodeScene::RenderContext::setScissor(const VkRect2D& scissor, bool setVp) {
    vkCmdSetScissor(renderContext.getCommandBuffer(), 0, 1, &scissor);
    if (setVp) {
        const VkViewport vp = makeViewport(scissor);
        vkCmdSetViewport(renderContext.getCommandBuffer(), 0, 1, &vp);
    }
}

void CodeScene::RenderContext::resetViewport() {
    setViewport(renderContext.parentViewport(), false);
}

void CodeScene::RenderContext::resetScissor() {
    setScissor(makeScissor(renderContext.parentViewport()), false);
}

void CodeScene::RenderContext::resetViewportAndScissor() {
    setViewport(renderContext.parentViewport(), true);
}

} // namespace scene
} // namespace rc
} // namespace bl
