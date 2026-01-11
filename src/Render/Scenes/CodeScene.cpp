#include <BLIB/Render/Scenes/CodeScene.hpp>

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config/RenderPhases.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene2DFactory.hpp>
#include <BLIB/Render/Events/SceneObjectRemoved.hpp>
#include <BLIB/Render/Lighting/Scene2DLighting.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/ShaderResources/ShaderResourceStore.hpp>

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
: Scene(engine)
, renderCallback(renderCallback)
, lighting(shaderInputStore.getShaderResourceWithKey(sri::Scene2DLightingKey)->getBuffer()[0]) {
    emitter.connect(engine.renderer().getSignalChannel());
}

CodeScene::~CodeScene() {
    // noop
}

void CodeScene::renderOpaqueObjects(scene::SceneRenderContext& context) {
    RenderContext ctx(context);
    renderCallback(ctx);
}

void CodeScene::renderTransparentObjects(scene::SceneRenderContext&) {}

void CodeScene::doObjectRemoval(SceneObject* object, mat::MaterialPipeline*) {
    CodeSceneObject* obj     = static_cast<CodeSceneObject*>(object);
    const ecs::Entity entity = obj->entity;

    obj->descriptors.releaseObject(obj->entity, obj->sceneKey);
    objects.release(obj->sceneKey);
    emitter.emit<rc::event::SceneObjectRemoved>({this, entity});
}

SceneObject* CodeScene::doAdd(ecs::Entity entity, rcom::DrawableBase& object,
                              UpdateSpeed updateFreq) {
    CodeSceneObject& obj = *objects.allocate(updateFreq, entity).newObject;

    obj.pipeline = object.getCurrentPipeline();
    obj.descriptors.init(this,
                         obj.pipeline->getPipeline(cfg::RenderPhases::Forward)->pipelineLayout());
    obj.descriptors.addObservers(targetTable);
    obj.descriptors.allocateObject(entity, obj.sceneKey);

    return &obj;
}

std::unique_ptr<cam::Camera> CodeScene::createDefaultCamera() {
    return std::make_unique<cam::Camera2D>(
        sf::FloatRect({0.f, 0.f},
                      sf::Vector2f(renderer.getObserver().getRegionSize().x,
                                   renderer.getObserver().getRegionSize().y)));
}

void CodeScene::setDefaultNearAndFarPlanes(cam::Camera& camera) const {
    camera.setNearAndFarPlanes(0.f, -1000.f);
}

void CodeScene::doBatchChange(const BatchChange& change, mat::MaterialPipeline* ogPipeline) {
    if (ogPipeline != change.newPipeline) {
        CodeSceneObject& object = *static_cast<CodeSceneObject*>(change.changed);
        object.pipeline         = change.newPipeline;
        object.descriptors.reinit(
            object.pipeline->getPipeline(cfg::RenderPhases::Forward)->pipelineLayout(),
            targetTable,
            object);
    }
}

CodeScene::RenderContext::RenderContext(SceneRenderContext& ctx)
: renderContext(ctx) {}

void CodeScene::RenderContext::renderObject(rcom::DrawableBase& object) {
    CodeSceneObject* obj = static_cast<CodeSceneObject*>(object.getSceneRef().object);

#ifdef BLIB_DEBUG
    if (!obj) { throw std::runtime_error("Cannot render object with nullptr scene ref"); }
#endif

    if (!object.isHidden()) {
        renderContext.bindPipeline(*obj->pipeline, object.getPipelineSpecialization());
        renderContext.bindDescriptors(
            obj->pipeline->getPipeline(cfg::RenderPhases::Forward)->pipelineLayout().rawLayout(),
            obj->sceneKey.updateFreq,
            obj->descriptors.get(renderContext.currentObserverIndex()).data(),
            obj->descriptors.getDescriptorSetCount());
        for (std::uint8_t i = obj->descriptors.getPerObjectStart();
             i < obj->descriptors.getDescriptorSetCount();
             ++i) {
            obj->descriptors.get(renderContext.currentObserverIndex())[i]->bindForObject(
                renderContext,
                obj->pipeline->getPipeline(cfg::RenderPhases::Forward)
                    ->pipelineLayout()
                    .rawLayout(),
                i,
                obj->sceneKey);
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

void CodeScene::doRegisterObserver(RenderTarget* target, std::uint32_t observerIndex) {
    objects.forEach([&](CodeSceneObject& obj) {
        obj.descriptors.addObserver(observerIndex, *target);
        obj.descriptors.allocateObject(observerIndex, obj.entity, obj.sceneKey);
    });
}

void CodeScene::doUnregisterObserver(RenderTarget*, std::uint32_t observerIndex) {
    objects.forEach([&](CodeSceneObject& obj) { obj.descriptors.removeObserver(observerIndex); });
}

} // namespace scene
} // namespace rc
} // namespace bl
