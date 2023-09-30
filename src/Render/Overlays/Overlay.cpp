#include <BLIB/Render/Overlays/Overlay.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
Overlay::Overlay(engine::Engine& e)
: Scene(e, objects.makeEntityCallback())
, engine(e)
, objects(e.ecs())
, scaler(engine.systems().getSystem<sys::OverlayScalerSystem>())
, cachedParentViewport{}
, cachedTargetSize{} {
    ecsPool = &engine.ecs().getAllComponents<ovy::OverlayObject>();
    roots.reserve(Config::DefaultSceneObjectCapacity / 4);
    renderStack.reserve(Config::DefaultSceneObjectCapacity / 2);
    toParent.reserve(32);
}

Overlay::~Overlay() { objects.unlinkAll(descriptorSets); }

void Overlay::renderScene(scene::SceneRenderContext& ctx) {
    // ensure parent-child relationships are applied
    for (const auto& pp : toParent) { applyParent(pp.first, pp.second); }
    toParent.clear();

    std::copy(roots.begin(), roots.end(), std::inserter(renderStack, renderStack.begin()));

    if (static_cast<std::uint32_t>(ctx.parentViewport().width) != cachedTargetSize.x ||
        static_cast<std::uint32_t>(ctx.parentViewport().height) != cachedTargetSize.y) {
        cachedParentViewport = ctx.parentViewport();
        cachedTargetSize.x   = static_cast<std::uint32_t>(ctx.parentViewport().width);
        cachedTargetSize.y   = static_cast<std::uint32_t>(ctx.parentViewport().height);
        refreshAll();
    }

    VkPipeline currentPipeline = nullptr;
    while (!renderStack.empty()) {
        // TODO - if we get weird rendering we may need a queue here
        ovy::OverlayObject& obj = *renderStack.back();
        renderStack.pop_back();

        if (obj.hidden) { continue; }

        obj.applyViewport(ctx.getCommandBuffer());

        const VkPipeline np = obj.pipeline->rawPipeline(ctx.currentRenderPass());
        if (np != currentPipeline) {
            currentPipeline = np;
            ctx.bindPipeline(*obj.pipeline);
            ctx.bindDescriptors(obj.pipeline->pipelineLayout().rawLayout(),
                                obj.sceneKey.updateFreq,
                                obj.descriptors.data(),
                                obj.descriptorCount);
        }
        for (std::uint8_t i = obj.perObjStart; i < obj.descriptorCount; ++i) {
            obj.descriptors[i]->bindForObject(
                ctx, obj.pipeline->pipelineLayout().rawLayout(), i, obj.sceneKey);
        }
        ctx.renderObject(obj);

        std::copy(obj.getChildren().begin(),
                  obj.getChildren().end(),
                  std::inserter(renderStack, renderStack.end()));
    }
}

scene::SceneObject* Overlay::doAdd(ecs::Entity entity, rcom::DrawableBase& object,
                                   UpdateSpeed updateFreq) {
    ovy::OverlayObject& obj = *objects.allocate(updateFreq, entity);
    obj.entity              = entity;
    obj.overlay             = this;
    obj.pipeline            = &renderer.pipelineCache().getPipeline(object.pipeline);
    obj.descriptorCount =
        obj.pipeline->pipelineLayout().initDescriptorSets(descriptorSets, obj.descriptors.data());
    obj.perObjStart     = obj.descriptorCount;
    obj.overlayViewport = &cachedParentViewport;
    for (unsigned int i = 0; i < obj.descriptorCount; ++i) {
        obj.descriptors[i]->allocateObject(entity, obj.sceneKey);
        if (!obj.descriptors[i]->isBindless()) {
            obj.perObjStart = std::min(obj.perObjStart, static_cast<std::uint8_t>(i));
        }
    }

    return &obj;
}

void Overlay::doRemove(scene::SceneObject* object, std::uint32_t) {
    ovy::OverlayObject* obj  = static_cast<ovy::OverlayObject*>(object);
    const ecs::Entity entity = objects.getObjectEntity(obj->sceneKey);

    for (unsigned int i = 0; i < obj->descriptorCount; ++i) {
        obj->descriptors[i]->releaseObject(entity, obj->sceneKey);
    }
    objects.release(obj->sceneKey);

    // TODO - move this to scene base and use ecs
    for (ovy::OverlayObject* child : obj->getChildren()) { removeObject(child); }

    if (!obj->hasParent()) {
        const auto it = std::find(roots.begin(), roots.end(), obj);
        if (it != roots.end()) { roots.erase(it); }
    }

    engine.ecs().removeComponent<ovy::OverlayObject>(entity);
}

void Overlay::doBatchChange(const BatchChange& change, std::uint32_t ogPipeline) {
    if (ogPipeline != change.newPipeline) {
        ovy::OverlayObject& object = *static_cast<ovy::OverlayObject*>(change.changed);
        object.pipeline            = &renderer.pipelineCache().getPipeline(change.newPipeline);
        const ecs::Entity entity   = objects.getObjectEntity(object.sceneKey);
        object.descriptorCount =
            object.pipeline->pipelineLayout().updateDescriptorSets(descriptorSets,
                                                                   object.descriptors.data(),
                                                                   object.descriptorCount,
                                                                   entity,
                                                                   object.sceneKey.sceneId,
                                                                   object.sceneKey.updateFreq);
    }
}

void Overlay::setParent(scene::Key child, ecs::Entity parent) {
    toParent.emplace_back(child, parent);
}

void Overlay::applyParent(scene::Key child, ecs::Entity parent) {
    const ecs::Entity cent  = objects.getObjectEntity(child);
    ovy::OverlayObject& obj = objects.getObject(child);

    if (parent == ecs::InvalidEntity) {
        engine.ecs().removeEntityParent(cent);
        roots.emplace_back(&obj);
    }
    else { engine.ecs().setEntityParent(cent, parent); }
}

void Overlay::refreshAll() {
    for (auto o : roots) { scaler.refreshObjectAndChildren(*o, cachedParentViewport); }
}

} // namespace rc
} // namespace bl
