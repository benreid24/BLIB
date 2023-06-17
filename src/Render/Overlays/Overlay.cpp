#include <BLIB/Render/Overlays/Overlay.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
Overlay::Overlay(Renderer& r, engine::Engine& e, std::uint32_t ms, std::uint32_t md)
: Scene(r, ms, md)
, engine(e)
, objects(ms + md)
, parentMap(ms + md, NoParent)
, cachedParentViewport{} {
    roots.reserve(std::max((ms + md) / 4, 4u));
    renderStack.reserve(roots.capacity() * 2);
    event::Dispatcher::subscribe(this);
}

void Overlay::renderScene(scene::SceneRenderContext& ctx) {
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
            ctx.bindDescriptors(
                obj.pipeline->pipelineLayout(), obj.descriptors.data(), obj.descriptorCount);
        }
        ctx.renderObject(obj.pipeline->pipelineLayout(), obj);

        std::copy(obj.children.begin(),
                  obj.children.end(),
                  std::inserter(renderStack, renderStack.end()));
    }
}

scene::SceneObject* Overlay::doAdd(ecs::Entity entity, std::uint32_t sceneId,
                                   UpdateSpeed updateFreq, const scene::StagePipelines& pipelines) {
    ovy::OverlayObject& obj = objects[sceneId];
    const auto pid          = pipelines[Config::SceneObjectStage::OpaquePass];

#ifdef BLIB_DEBUG
    if (pid == Config::PipelineIds::None) {
        BL_LOG_ERROR << "Use OpaquePass for overlay pipelines";
        throw std::runtime_error("Use OpaquePass for overlay pipelines");
    }
#endif

    obj.pipeline        = &renderer.pipelineCache().getPipeline(pid);
    obj.descriptorCount = obj.pipeline->initDescriptorSets(descriptorSets, obj.descriptors.data());
    for (unsigned int i = 0; i < obj.descriptorCount; ++i) {
        obj.descriptors[i]->allocateObject(sceneId, entity, updateFreq);
    }
    obj.viewport.assign(engine.ecs(), entity);
    obj.scaler.assign(engine.ecs(), entity);
    entityToSceneId.try_emplace(entity, sceneId);

    return &obj;
}

void Overlay::doRemove(ecs::Entity entity, scene::SceneObject* object,
                       const scene::StagePipelines&) {
    ovy::OverlayObject* obj = static_cast<ovy::OverlayObject*>(object);
    const std::uint32_t id  = obj - objects.data();

    for (unsigned int i = 0; i < obj->descriptorCount; ++i) {
        obj->descriptors[i]->releaseObject(id, entity);
    }

    obj->viewport.release();
    obj->scaler.release();

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

void Overlay::setParent(std::uint32_t child, std::uint32_t parent) {
    parentMap[child] = parent;
    if (parent != NoParent) {
        objects[parent].registerChild(child);
        objects[child].refreshViewport(cachedParentViewport, objects[parent].cachedViewport);
    }
    else {
        roots.emplace_back(child);
        objects[child].refreshViewport(cachedParentViewport, cachedParentViewport);
    }
}

void Overlay::refreshAll() {
    for (std::uint32_t o : roots) { refreshObjectAndChildren(o); }
}

void Overlay::refreshObjectAndChildren(std::uint32_t id) {
    const std::uint32_t pid = parentMap[id];
    ovy::OverlayObject& obj = objects[id];
    const VkViewport& vp    = pid != NoParent ? obj.cachedViewport : cachedParentViewport;
    obj.refreshViewport(cachedParentViewport, vp);
    if (obj.scaler.valid()) {
        obj.scaler.get().setTargetSize({obj.cachedViewport.width, obj.cachedViewport.height});
    }
    for (std::uint32_t child : obj.children) { refreshObjectAndChildren(child); }
}

void Overlay::observe(const ecs::event::ComponentAdded<ovy::Viewport>& event) {
    const auto it = entityToSceneId.find(event.entity);
    if (it != entityToSceneId.end()) { refreshObjectAndChildren(it->second); }
}

void Overlay::observe(const ecs::event::ComponentRemoved<ovy::Viewport>& event) {
    const auto it = entityToSceneId.find(event.entity);
    if (it != entityToSceneId.end()) { refreshObjectAndChildren(it->second); }
}

void Overlay::observe(const ovy::ViewportChanged& event) {
    if (event.overlay == this) { refreshObjectAndChildren(event.sceneId); }
}

} // namespace render
} // namespace bl
