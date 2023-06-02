#include <BLIB/Render/Overlays/Overlay.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
Overlay::Overlay(Renderer& r, std::uint32_t ms, std::uint32_t md)
: Scene(r, ms, md)
, objects(ms + md)
, parentMap(ms + md, NoParent) {
    roots.reserve(std::max((ms + md) / 4, 4u));
    renderStack.reserve(roots.capacity() * 2);
}

void Overlay::renderScene(scene::SceneRenderContext& ctx) {
    std::copy(roots.begin(), roots.end(), std::inserter(renderStack, renderStack.begin()));

    VkPipeline currentPipeline = nullptr;
    while (!renderStack.empty()) {
        const std::uint32_t oid = renderStack.back();
        renderStack.pop_back();

        if (oid == PopViewport) {
            // TODO - pop viewort
            continue;
        }

        ovy::OverlayObject& obj = objects[oid];

        // TODO - set/push scissor/viewport

        if (obj.pipeline->rawPipeline() != currentPipeline) {
            currentPipeline = obj.pipeline->rawPipeline();
            ctx.bindDescriptors(
                obj.pipeline->pipelineLayout(), obj.descriptors.data(), obj.descriptorCount);
        }
        ctx.renderObject(obj.pipeline->pipelineLayout(), obj);

        std::copy(obj.children.begin(),
                  obj.children.end(),
                  std::inserter(renderStack, renderStack.end()));
    }
}

scene::SceneObject* Overlay::doAdd(ecs::Entity, std::uint32_t sceneId, UpdateSpeed,
                                   const scene::StagePipelines& pipelines) {
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

    return &obj;
}

void Overlay::doRemove(ecs::Entity entity, scene::SceneObject* object,
                       const scene::StagePipelines&) {
    ovy::OverlayObject* obj = static_cast<ovy::OverlayObject*>(object);
    const std::uint32_t id  = obj - objects.data();

    for (unsigned int i = 0; i < obj->descriptorCount; ++i) {
        obj->descriptors[i]->releaseObject(id, entity);
    }

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

} // namespace render
} // namespace bl