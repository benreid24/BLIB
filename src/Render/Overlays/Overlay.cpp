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
, scaler(engine.systems().getSystem<sys::OverlayScaler>())
, objects()
, cachedParentViewport{} {
    roots.reserve(Config::DefaultSceneObjectCapacity / 4);
    renderStack.reserve(Config::DefaultSceneObjectCapacity / 2);
    toParent.reserve(32);
    scaler.registerOverlay(this);
}

Overlay::~Overlay() {
    scaler.removeOverlay(this);
    objects.unlinkAll(descriptorSets);
}

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
        const scene::Key key = renderStack.back();
        renderStack.pop_back();

        ovy::OverlayObject& obj = objects.getObject(key);
        if (obj.hidden) { continue; }

        obj.applyViewport(ctx.getCommandBuffer());

        const VkPipeline np = obj.pipeline->rawPipeline(ctx.currentRenderPass());
        if (np != currentPipeline) {
            currentPipeline = np;
            ctx.bindPipeline(*obj.pipeline);
            ctx.bindDescriptors(obj.pipeline->pipelineLayout().rawLayout(),
                                key.updateFreq,
                                obj.descriptors.data(),
                                obj.descriptorCount);
        }
        for (std::uint8_t i = obj.perObjStart; i < obj.descriptorCount; ++i) {
            obj.descriptors[i]->bindForObject(
                ctx, obj.pipeline->pipelineLayout().rawLayout(), i, key);
        }
        ctx.renderObject(obj);

        std::copy(obj.children.begin(),
                  obj.children.end(),
                  std::inserter(renderStack, renderStack.end()));
    }
}

scene::SceneObject* Overlay::doAdd(ecs::Entity entity, rcom::DrawableBase& object,
                                   UpdateSpeed updateFreq) {
    auto alloc = objects.allocate(updateFreq, entity);
    if (alloc.addressesChanged) {
        auto& index = updateFreq == UpdateSpeed::Static ? staticIndex : dynamicIndex;
        index.parentMap.resize(alloc.newCapacity, {UpdateSpeed::Static, NoParent});
    }

    ovy::OverlayObject& obj = *alloc.newObject;
    obj.pipeline            = &renderer.pipelineCache().getPipeline(object.pipeline);
    obj.descriptorCount =
        obj.pipeline->pipelineLayout().initDescriptorSets(descriptorSets, obj.descriptors.data());
    obj.perObjStart = obj.descriptorCount;
    for (unsigned int i = 0; i < obj.descriptorCount; ++i) {
        obj.descriptors[i]->allocateObject(entity, obj.sceneKey);
        if (!obj.descriptors[i]->isBindless()) {
            obj.perObjStart = std::min(obj.perObjStart, static_cast<std::uint8_t>(i));
        }
    }
    obj.scaler.assign(engine.ecs(), entity);
    obj.viewport.assign(engine.ecs(), entity);
    entityToSceneId.try_emplace(entity, obj.sceneKey);

    return &obj;
}

void Overlay::doRemove(scene::SceneObject* object, std::uint32_t) {
    ovy::OverlayObject* obj  = static_cast<ovy::OverlayObject*>(object);
    const ecs::Entity entity = objects.getObjectEntity(obj->sceneKey);

    for (unsigned int i = 0; i < obj->descriptorCount; ++i) {
        obj->descriptors[i]->releaseObject(entity, obj->sceneKey);
    }

    obj->scaler.release();
    obj->viewport.release();

    for (scene::Key child : obj->children) { removeObject(&objects.getObject(child)); }
    obj->children.clear();

    TreeIndex& index = obj->sceneKey.updateFreq == UpdateSpeed::Static ? staticIndex : dynamicIndex;
    const scene::Key parent = index.parentMap[obj->sceneKey.sceneId];
    if (parent.sceneId == NoParent) {
        for (auto it = roots.begin(); it != roots.end(); ++it) {
            if (*it == obj->sceneKey) {
                roots.erase(it);
                break;
            }
        }
    }
    else { objects.getObject(parent).removeChild(obj->sceneKey); }
    entityToSceneId.erase(entity);
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
    const auto it = entityToSceneId.find(parent);
    if (parent != ecs::InvalidEntity && it == entityToSceneId.end()) {
        BL_LOG_WARN << "Invalid parent " << parent << " for entity "
                    << objects.getObjectEntity(child) << " (scene id: " << child.sceneId << ")";
    }
    const scene::Key pid =
        it != entityToSceneId.end() ? it->second : scene::Key{UpdateSpeed::Static, NoParent};
    TreeIndex& index = child.updateFreq == UpdateSpeed::Static ? staticIndex : dynamicIndex;

    index.parentMap[child.sceneId] = pid;
    if (pid.sceneId != NoParent) { objects.getObject(pid).registerChild(child); }
    else { roots.emplace_back(child); }
    refreshObjectAndChildren(child);
}

void Overlay::refreshAll() {
    for (auto o : roots) { refreshObjectAndChildren(o); }
}

void Overlay::refreshObjectAndChildren(scene::Key id) {
    TreeIndex& index        = id.updateFreq == UpdateSpeed::Static ? staticIndex : dynamicIndex;
    const scene::Key pid    = index.parentMap[id.sceneId];
    ovy::OverlayObject& obj = objects.getObject(id);
    const VkViewport& vp =
        pid.sceneId != NoParent ? objects.getObject(pid).cachedViewport : cachedParentViewport;
    scaler.refreshEntity(objects.getObjectEntity(id), vp);
    obj.refreshViewport(cachedParentViewport, vp);
    for (auto child : obj.children) { refreshObjectAndChildren(child); }
}

void Overlay::refreshScales() {
    std::copy(roots.begin(), roots.end(), std::inserter(renderStack, renderStack.begin()));

    while (!renderStack.empty()) {
        const scene::Key oid = renderStack.back();
        renderStack.pop_back();
        ovy::OverlayObject& obj = objects.getObject(oid);

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

} // namespace rc
} // namespace bl
