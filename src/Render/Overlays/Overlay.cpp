#include <BLIB/Render/Overlays/Overlay.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
Overlay::Overlay(Renderer& r, engine::Engine& e, std::uint32_t ms, std::uint32_t md)
: Scene(r, ms, md)
, engine(e)
, scaler(engine.systems().getSystem<sys::OverlayScaler>())
, objects(ms + md)
, parentMap(ms + md, NoParent)
, cachedParentViewport{} {
    roots.reserve(std::max((ms + md) / 4, 4u));
    renderStack.reserve(roots.capacity() * 2);
    toParent.reserve(32);
    scaler.registerOverlay(this);
}

Overlay::~Overlay() { scaler.removeOverlay(this); }

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
        const std::uint32_t oid = renderStack.back();
        renderStack.pop_back();

        ovy::OverlayObject& obj = objects[oid];
        if (obj.hidden) { continue; }

        obj.applyViewport(ctx.getCommandBuffer());

        const VkPipeline np = obj.pipeline->rawPipeline(ctx.currentRenderPass());
        if (np != currentPipeline) {
            currentPipeline = np;
            ctx.bindPipeline(*obj.pipeline);
            ctx.bindDescriptors(obj.pipeline->pipelineLayout().rawLayout(),
                                obj.descriptors.data(),
                                obj.descriptorCount);
        }
        ctx.renderObject(obj.pipeline->pipelineLayout().rawLayout(), obj);

        std::copy(obj.children.begin(),
                  obj.children.end(),
                  std::inserter(renderStack, renderStack.end()));
    }
}

scene::SceneObject* Overlay::doAdd(ecs::Entity entity, com::DrawableBase& object,
                                   std::uint32_t sceneId, UpdateSpeed updateFreq) {
    ovy::OverlayObject& obj = objects[sceneId];
    obj.pipeline            = &renderer.pipelineCache().getPipeline(object.pipeline);
    obj.descriptorCount =
        obj.pipeline->pipelineLayout().initDescriptorSets(descriptorSets, obj.descriptors.data());
    for (unsigned int i = 0; i < obj.descriptorCount; ++i) {
        obj.descriptors[i]->allocateObject(sceneId, entity, updateFreq);
    }
    obj.scaler.assign(engine.ecs(), entity);
    obj.viewport.assign(engine.ecs(), entity);
    entityToSceneId.try_emplace(entity, sceneId);

    return &obj;
}

void Overlay::doRemove(ecs::Entity entity, scene::SceneObject* object, std::uint32_t) {
    ovy::OverlayObject* obj = static_cast<ovy::OverlayObject*>(object);
    const std::uint32_t id  = obj - objects.data();

    for (unsigned int i = 0; i < obj->descriptorCount; ++i) {
        obj->descriptors[i]->releaseObject(id, entity);
    }

    obj->scaler.release();
    obj->viewport.release();

    for (std::uint32_t child : obj->children) { removeObject(&objects[child]); }
    obj->children.clear();

    const std::uint32_t parent = parentMap[id];
    if (parent == NoParent) {
        for (auto it = roots.begin(); it != roots.end(); ++it) {
            if (*it == id) {
                roots.erase(it);
                break;
            }
        }
    }
    else { objects[parent].removeChild(id); }
    entityToSceneId.erase(entity);
}

void Overlay::doBatchChange(const BatchChange& change, std::uint32_t ogPipeline) {
    if (ogPipeline != change.newPipeline) {
        ovy::OverlayObject& object = *static_cast<ovy::OverlayObject*>(change.changed);
        object.pipeline            = &renderer.pipelineCache().getPipeline(change.newPipeline);
        const ecs::Entity entity   = getEntityFromId(object.sceneId);
        const UpdateSpeed speed    = getObjectSpeed(object.sceneId);
        object.descriptorCount =
            object.pipeline->pipelineLayout().updateDescriptorSets(descriptorSets,
                                                                   object.descriptors.data(),
                                                                   object.descriptorCount,
                                                                   entity,
                                                                   object.sceneId,
                                                                   speed);
    }
}

void Overlay::setParent(std::uint32_t child, ecs::Entity parent) {
    toParent.emplace_back(child, parent);
}

void Overlay::applyParent(std::uint32_t child, ecs::Entity parent) {
    const auto it = entityToSceneId.find(parent);
    if (parent != ecs::InvalidEntity && it == entityToSceneId.end()) {
        BL_LOG_WARN << "Invalid parent " << parent << " for entity " << getEntityFromId(child)
                    << " (scene id: " << child << ")";
    }
    const std::uint32_t pid = it != entityToSceneId.end() ? it->second : NoParent;

    parentMap[child] = pid;
    if (pid != NoParent) { objects[pid].registerChild(child); }
    else { roots.emplace_back(child); }
    refreshObjectAndChildren(child);
}

void Overlay::refreshAll() {
    for (std::uint32_t o : roots) { refreshObjectAndChildren(o); }
}

void Overlay::refreshObjectAndChildren(std::uint32_t id) {
    const std::uint32_t pid = parentMap[id];
    ovy::OverlayObject& obj = objects[id];
    const VkViewport& vp    = pid != NoParent ? objects[pid].cachedViewport : cachedParentViewport;
    scaler.refreshEntity(getEntityFromId(id), vp);
    obj.refreshViewport(cachedParentViewport, vp);
    for (std::uint32_t child : obj.children) { refreshObjectAndChildren(child); }
}

void Overlay::refreshScales() {
    std::copy(roots.begin(), roots.end(), std::inserter(renderStack, renderStack.begin()));

    while (!renderStack.empty()) {
        const std::uint32_t oid = renderStack.back();
        renderStack.pop_back();
        ovy::OverlayObject& obj = objects[oid];

        if (obj.scaler.valid()) {
            if (obj.scaler.get().isDirty()) { refreshObjectAndChildren(oid); }
        }
        else {
            std::copy(obj.children.begin(),
                      obj.children.end(),
                      std::inserter(renderStack, renderStack.end()));
        }
    }
}

} // namespace gfx
} // namespace bl
