#include <BLIB/Render/Overlays/Overlay.hpp>

#include <BLIB/Cameras/OverlayCamera.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config/Constants.hpp>
#include <BLIB/Render/Config/RenderPhases.hpp>
#include <BLIB/Render/Events/SceneObjectRemoved.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace
{
rg::Strategy* strategy = nullptr;
} // namespace

Overlay::Overlay(engine::Engine& e)
: Scene(e)
, objects(e.ecs())
, scaler(engine.systems().getSystem<sys::OverlayScalerSystem>())
, cachedParentViewport{}
, cachedTargetSize{}
, needRefreshAll(false) {
    ecsPool = &engine.ecs().getAllComponents<ovy::OverlayObject>();
    roots.reserve(cfg::Constants::DefaultSceneObjectCapacity / 4);
    renderStack.reserve(cfg::Constants::DefaultSceneObjectCapacity / 2);
    emitter.connect(engine.renderer().getSignalChannel());
    subscribe(e.renderer().getSignalChannel());
}

Overlay::~Overlay() {
    // reset viewports to prevent read into invalid memory
    for (ovy::OverlayObject* obj : all) { obj->overlayViewport = nullptr; }
}

void Overlay::renderOpaqueObjects(scene::SceneRenderContext& ctx) {
    std::unique_lock lock(objectMutex);
    ecs::Transaction<ecs::tx::EntityUnlocked, ecs::tx::ComponentRead<ovy::OverlayObject>>
        transaction(engine.ecs());

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

        if (obj.component->isHidden()) { continue; }

        const std::uint32_t spec = obj.component->getPipelineSpecialization();
        if (!obj.entity.flagSet(ecs::Flags::Dummy)) {
            vkCmdSetScissor(ctx.getCommandBuffer(), 0, 1, &obj.cachedScissor);

            const vk::PipelineLayout& layout =
                obj.pipeline->getPipeline(cfg::RenderPhases::Overlay)->pipelineLayout();
            const VkPipelineLayout vkl = layout.rawLayout();
            const VkPipeline vkp =
                obj.pipeline->getRawPipeline(ctx.getRenderPhase(), ctx.currentRenderPass(), spec);
            if (vkl != currentPipelineLayout || currentSpeed != obj.sceneKey.updateFreq) {
                currentSpeed          = obj.sceneKey.updateFreq;
                currentPipelineLayout = vkl;
                currentPipeline       = vkp;
                ctx.bindPipeline(*obj.pipeline, spec);
                ctx.bindDescriptors(vkl,
                                    obj.sceneKey.updateFreq,
                                    obj.descriptors.get(ctx.currentObserverIndex()).data(),
                                    obj.descriptors.getDescriptorSetCount());
            }
            else if (currentPipeline != vkp) {
                currentPipeline = vkp;
                ctx.bindPipeline(*obj.pipeline, spec);
            }
            for (std::uint8_t i = obj.descriptors.getPerObjectStart();
                 i < obj.descriptors.getDescriptorSetCount();
                 ++i) {
                obj.descriptors.get(ctx.currentObserverIndex())[i]->bindForObject(
                    ctx, vkl, i, obj.sceneKey);
            }
            ctx.renderObject(obj);
        }

        std::copy(obj.getChildren().rbegin(),
                  obj.getChildren().rend(),
                  std::inserter(renderStack, renderStack.end()));
    }
}

scene::SceneObject* Overlay::doAdd(ecs::Entity entity, rcom::DrawableBase& object,
                                   UpdateSpeed updateFreq) {
    ovy::OverlayObject& obj = *objects.allocate(updateFreq, entity);

    obj.entity   = entity;
    obj.overlay  = this;
    obj.pipeline = object.getCurrentPipeline();
    obj.descriptors.init(this,
                         obj.pipeline->getPipeline(cfg::RenderPhases::Overlay)->pipelineLayout());
    obj.descriptors.addObservers(targetTable);
    obj.descriptors.allocateObject(entity, obj.sceneKey);
    obj.overlayViewport = &cachedParentViewport;

    if (!engine.ecs().entityHasParent(entity)) {
        roots.emplace_back(&obj);
        sortRoots();
    }

    com::OverlayScaler* scaler = engine.ecs().getComponent<com::OverlayScaler>(entity);
    if (scaler) { scaler->dirty = true; }

    util::ReadWriteLock::WriteScopeGuard writeLock(allListLock);
    all.emplace_back(&obj);

    return &obj;
}

void Overlay::doObjectRemoval(scene::SceneObject* object, mat::MaterialPipeline*) {
    ovy::OverlayObject* obj = static_cast<ovy::OverlayObject*>(object);
    obj->overlayViewport    = nullptr;

    removeFromAll(obj);

    auto childCopy = obj->getChildren();
    for (ovy::OverlayObject* child : childCopy) {
        if (engine.ecs().getEntityParentDestructionBehavior(child->entity) !=
                ecs::ParentDestructionBehavior::OrphanedByParent &&
            child->sceneKey.sceneId != scene::Key::InvalidSceneId) {
            removeObject(child);
        }
    }

    const auto it = std::find(roots.begin(), roots.end(), obj);
    if (it != roots.end()) {
        roots.erase(it);
        sortRoots();
    }

    obj->descriptors.releaseObject(obj->entity, obj->sceneKey);
    objects.release(obj->sceneKey);
    engine.ecs().removeComponent<ovy::OverlayObject>(obj->entity);
    emitter.emit<rc::event::SceneObjectRemoved>({this, obj->entity});
}

void Overlay::doBatchChange(const BatchChange& change, mat::MaterialPipeline* ogPipeline) {
    if (ogPipeline != change.newPipeline) {
        ovy::OverlayObject& object = *static_cast<ovy::OverlayObject*>(change.changed);
        object.pipeline            = change.newPipeline;
        const ecs::Entity entity   = object.entity;
        object.descriptors.reinit(
            object.pipeline->getPipeline(cfg::RenderPhases::Overlay)->pipelineLayout(),
            targetTable,
            object);
    }
}

void Overlay::refreshAll() {
    for (auto o : roots) { scaler.refreshObjectAndChildren(*o); }
}

void Overlay::process(const ecs::event::EntityParentSet& event) {
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

void Overlay::process(const ecs::event::EntityParentRemoved& event) {
    if (event.orphanIsBeingDestroyed) { return; }
    ovy::OverlayObject* obj = ecsPool->get(event.orphan);
    if (!obj || obj->overlay != this) { return; }

    if (std::find(roots.begin(), roots.end(), obj) != roots.end()) { return; }
    roots.emplace_back(obj);
    sortRoots();
    needRefreshAll = true;
}

void Overlay::process(const ecs::event::ComponentRemoved<ovy::OverlayObject>& event) {
    if (event.component.overlay == this) {
        ovy::OverlayObject* obj = const_cast<ovy::OverlayObject*>(&event.component);
        const auto it           = std::find(roots.begin(), roots.end(), obj);
        if (it != roots.end()) { roots.erase(it); }
        removeFromAll(obj);
    }
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

void Overlay::useRenderStrategy(rg::Strategy* ns) { strategy = ns; }

rg::Strategy* Overlay::getRenderStrategy() { return strategy; }

void Overlay::doRegisterObserver(RenderTarget* target, std::uint32_t observerIndex) {
    util::ReadWriteLock::ReadScopeGuard guard(allListLock);

    for (ovy::OverlayObject* obj : all) {
        obj->descriptors.addObserver(observerIndex, *target);
        obj->descriptors.allocateObject(observerIndex, obj->entity, obj->sceneKey);
    }
}

void Overlay::doUnregisterObserver(RenderTarget*, std::uint32_t observerIndex) {
    util::ReadWriteLock::ReadScopeGuard guard(allListLock);

    for (ovy::OverlayObject* obj : all) { obj->descriptors.removeObserver(observerIndex); }
}

void Overlay::removeFromAll(ovy::OverlayObject* obj) {
    util::ReadWriteLock::WriteScopeGuard guard(allListLock);
    std::erase_if(all, [obj](ovy::OverlayObject* cmp) { return obj == cmp; });
}

} // namespace rc
} // namespace bl
