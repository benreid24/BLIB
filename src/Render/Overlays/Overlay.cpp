#include <BLIB/Render/Overlays/Overlay.hpp>

#include <BLIB/Cameras/OverlayCamera.hpp>
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
, cachedTargetSize{}
, needRefreshAll(false) {
    ecsPool = &engine.ecs().getAllComponents<ovy::OverlayObject>();
    roots.reserve(Config::DefaultSceneObjectCapacity / 4);
    renderStack.reserve(Config::DefaultSceneObjectCapacity / 2);
    bl::event::Dispatcher::subscribe(this);
}

Overlay::~Overlay() { objects.unlinkAll(descriptorSets); }

void Overlay::renderScene(scene::SceneRenderContext& ctx) {
    std::copy(roots.begin(), roots.end(), std::inserter(renderStack, renderStack.begin()));

    if (needRefreshAll ||
        static_cast<std::uint32_t>(ctx.parentViewport().width) != cachedTargetSize.x ||
        static_cast<std::uint32_t>(ctx.parentViewport().height) != cachedTargetSize.y) {
        cachedParentViewport = ctx.parentViewport();
        cachedTargetSize.x   = static_cast<std::uint32_t>(ctx.parentViewport().width);
        cachedTargetSize.y   = static_cast<std::uint32_t>(ctx.parentViewport().height);
        needRefreshAll       = false;
        refreshAll();
    }

    VkPipelineLayout currentPipelineLayout = nullptr;
    VkPipeline currentPipeline             = nullptr;
    UpdateSpeed currentSpeed{};
    while (!renderStack.empty()) {
        ovy::OverlayObject& obj = *renderStack.back();
        renderStack.pop_back();

        if (obj.hidden) { continue; }

        vkCmdSetScissor(ctx.getCommandBuffer(), 0, 1, &obj.cachedScissor);

        const vk::PipelineLayout& layout = obj.pipeline->pipelineLayout();
        const VkPipelineLayout vkl       = layout.rawLayout();
        const VkPipeline vkp             = obj.pipeline->rawPipeline(ctx.currentRenderPass());
        if (vkl != currentPipelineLayout || currentSpeed != obj.sceneKey.updateFreq) {
            currentSpeed          = obj.sceneKey.updateFreq;
            currentPipelineLayout = vkl;
            ctx.bindPipeline(*obj.pipeline);
            ctx.bindDescriptors(obj.pipeline->pipelineLayout().rawLayout(),
                                obj.sceneKey.updateFreq,
                                obj.descriptors.data(),
                                obj.descriptorCount);
        }
        else if (currentPipeline != vkp) {
            currentPipeline = vkp;
            ctx.bindPipeline(*obj.pipeline);
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

    if (!engine.ecs().entityHasParent(entity)) {
        roots.emplace_back(&obj);
        sortRoots();
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

    for (ovy::OverlayObject* child : obj->getChildren()) { removeObject(child); }

    if (!obj->hasParent()) {
        const auto it = std::find(roots.begin(), roots.end(), obj);
        if (it != roots.end()) {
            roots.erase(it);
            sortRoots();
        }
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

void Overlay::refreshAll() {
    for (auto o : roots) { scaler.refreshObjectAndChildren(*o); }
}

void Overlay::observe(const ecs::event::EntityParentSet& event) {
    ovy::OverlayObject* obj = ecsPool->get(event.child);
    if (!obj || obj->overlay != this) { return; }

    for (auto it = roots.begin(); it != roots.end(); ++it) {
        if (*it == obj) {
            roots.erase(it);
            sortRoots();
            needRefreshAll = true;
            break;
        }
    }
}

void Overlay::observe(const ecs::event::EntityParentRemoved& event) {
    ovy::OverlayObject* obj = ecsPool->get(event.orphan);
    if (!obj || obj->overlay != this) { return; }

    if (std::find(roots.begin(), roots.end(), obj) != roots.end()) { return; }
    roots.emplace_back(obj);
    sortRoots();
    needRefreshAll = true;
}

std::unique_ptr<cam::Camera> Overlay::createDefaultCamera() {
    return std::make_unique<cam::OverlayCamera>();
}

void Overlay::sortRoots() {
    std::sort(
        roots.begin(), roots.end(), [this](ovy::OverlayObject* left, ovy::OverlayObject* right) {
            com::Transform2D* lpos = engine.ecs().getComponent<com::Transform2D>(left->entity);
            com::Transform2D* rpos = engine.ecs().getComponent<com::Transform2D>(right->entity);
            if (!lpos || !rpos) { return left < right; }
            return lpos->getGlobalDepth() < rpos->getGlobalDepth();
        });
}

} // namespace rc
} // namespace bl
