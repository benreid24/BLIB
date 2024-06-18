#include <BLIB/Render/Overlays/Overlay.hpp>

#include <BLIB/Cameras/OverlayCamera.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Events/SceneObjectRemoved.hpp>
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

        if (!obj.entity.flagSet(ecs::Flags::Dummy)) {
            vkCmdSetScissor(ctx.getCommandBuffer(), 0, 1, &obj.cachedScissor);

            const vk::PipelineLayout& layout = obj.pipeline->pipelineLayout();
            const VkPipelineLayout vkl       = layout.rawLayout();
            const VkPipeline vkp             = obj.pipeline->rawPipeline(ctx.currentRenderPass());
            if (vkl != currentPipelineLayout || currentSpeed != obj.sceneKey.updateFreq) {
                currentSpeed          = obj.sceneKey.updateFreq;
                currentPipelineLayout = vkl;
                currentPipeline       = vkp;
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
    obj.pipeline = &renderer.pipelineCache().getPipeline(object.pipeline);
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

    com::OverlayScaler* scaler = engine.ecs().getComponent<com::OverlayScaler>(entity);
    if (scaler) { scaler->dirty = true; }

    return &obj;
}

void Overlay::doObjectRemoval(scene::SceneObject* object, std::uint32_t) {
    ovy::OverlayObject* obj = static_cast<ovy::OverlayObject*>(object);

    auto childCopy = obj->getChildren();
    for (ovy::OverlayObject* child : childCopy) {
        if (engine.ecs().getEntityParentDestructionBehavior(child->entity) !=
            ecs::ParentDestructionBehavior::OrphanedByParent) {
            removeObject(child);
        }
    }

    const auto it = std::find(roots.begin(), roots.end(), obj);
    if (it != roots.end()) {
        roots.erase(it);
        sortRoots();
    }

    for (unsigned int i = 0; i < obj->descriptorCount; ++i) {
        obj->descriptors[i]->releaseObject(obj->entity, object->sceneKey);
    }
    objects.release(obj->sceneKey);
    engine.ecs().removeComponent<ovy::OverlayObject>(obj->entity);
    bl::event::Dispatcher::dispatch<rc::event::SceneObjectRemoved>({this, obj->entity});
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
        object.perObjStart = object.descriptorCount;
        for (unsigned int i = 0; i < object.descriptorCount; ++i) {
            if (!object.descriptors[i]->isBindless()) {
                object.perObjStart = i;
                break;
            }
        }
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

void Overlay::observe(const ecs::event::ComponentRemoved<ovy::OverlayObject>& event) {
    if (event.component.overlay == this) {
        ovy::OverlayObject* obj = const_cast<ovy::OverlayObject*>(&event.component);
        const auto it           = std::find(roots.begin(), roots.end(), obj);
        if (it != roots.end()) { roots.erase(it); }
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

} // namespace rc
} // namespace bl
