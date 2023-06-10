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
, parentMap(ms + md, NoParent) {
    roots.reserve(std::max((ms + md) / 4, 4u));
    renderStack.reserve(roots.capacity() * 2);
    viewportStack.reserve(renderStack.capacity());
}

void Overlay::renderScene(scene::SceneRenderContext& ctx) {
    std::copy(roots.begin(), roots.end(), std::inserter(renderStack, renderStack.begin()));
    viewportStack.clear();
    viewportStack.emplace_back(ctx.parentViewport());

    VkPipeline currentPipeline = nullptr;
    while (!renderStack.empty()) {
        const std::uint32_t oid = renderStack.back();
        renderStack.pop_back();

        if (oid == PopViewport) {
            viewportStack.pop_back();
            viewportStack.back().apply(ctx.getCommandBuffer());
            continue;
        }

        ovy::OverlayObject& obj = objects[oid];
        if (obj.hidden) { continue; }

        if (obj.viewport.valid()) {
            viewportStack.emplace_back(obj.viewport.get().createViewport(
                ctx.parentViewport(), viewportStack.back().viewport));
            viewportStack.back().apply(ctx.getCommandBuffer());
            renderStack.emplace_back(PopViewport);
        }

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
}

void Overlay::setParent(std::uint32_t child, std::uint32_t parent) {
    parentMap[child] = parent;
    if (parent != NoParent) { objects[parent].registerChild(child); }
    else { roots.emplace_back(child); }
}

Overlay::ViewportPair::ViewportPair(const VkViewport& vp)
: viewport(vp)
, scissor(ovy::Viewport::viewportToScissor(vp)) {}

void Overlay::ViewportPair::apply(VkCommandBuffer commandBuffer) {
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

} // namespace render
} // namespace bl