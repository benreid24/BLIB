#include <BLIB/Render/Renderer/Scene.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderables/Renderable.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
using RpId = Config::RenderPassIds;

Scene::Scene(Renderer& r)
: renderer(r)
, objects(container::ObjectPool<Object>::GrowthPolicy::ExpandBuffer, 256)
, shadowPass(renderer, RpId::Shadow)
, opaquePass(renderer, RpId::Opaque)
, transparencyPass(renderer, RpId::Transparent)
, postFxPass(renderer, RpId::PostFX)
, overlayPass(renderer, RpId::Overlay) {
    renderPasses[RpId::Shadow]      = &shadowPass;
    renderPasses[RpId::Opaque]      = &opaquePass;
    renderPasses[RpId::Transparent] = &transparencyPass;
    renderPasses[RpId::PostFX]      = &postFxPass;
    renderPasses[RpId::Overlay]     = &overlayPass;
    toRemove.reserve(64);
}

Object::Handle Scene::createAndAddObject(Renderable* owner) {
    std::unique_lock lock(mutex);
    return objects.getStableRef(objects.emplace(owner));
}

void Scene::removeObject(const Object::Handle& obj) {
    std::unique_lock lock(eraseMutex);
    toRemove.emplace_back(obj);
}

void Scene::recordRenderCommands(const RenderFrame& target, VkCommandBuffer commandBuffer) {
    std::unique_lock lock(mutex);
    performRemovals();

    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (it->flags.isRenderPassDirty()) {
            updatePassMembership(objects.getStableRef(it));
            it->flags.markRenderPassesClean();
        }
    }

    // TODO - perform actual rendering of each pass and stage
    VkClearValue clearColors[1];
    clearColors[0].color = {0.f, 0.f, 0.f, 1.f};
    opaquePass.recordRenderCommands(commandBuffer, target, clearColors, std::size(clearColors));
}

void Scene::performRemovals() {
    std::unique_lock lock(eraseMutex);
    for (Object::Handle& obj : toRemove) {
        for (std::uint32_t rpid = 0; rpid < RpId::Count; ++rpid) {
            const std::uint32_t pid = obj->owner->passMembership.getPipelineForRenderPass(rpid);
            if (pid != Config::PipelineIds::None) { renderPasses[rpid]->removeObject(obj, pid); }
        }
        obj.erase();
    }
    toRemove.clear();
}

void Scene::updatePassMembership(Object::Handle& obj) {
    auto& passes = obj->owner->passMembership;
    while (passes.hasDiff()) {
        const auto diff = passes.nextDiff();
        switch (diff.type) {
        case RenderPassMembership::Diff::Add:
            renderPasses[diff.renderPassId]->addObject(obj, diff.pipelineId);
            break;
        case RenderPassMembership::Diff::Edit:
            renderPasses[diff.renderPassId]->changePipeline(
                obj, passes.getPipelineForRenderPass(diff.renderPassId), diff.pipelineId);
            break;
        case RenderPassMembership::Diff::Remove:
            renderPasses[diff.renderPassId]->removeObject(obj, diff.pipelineId);
            break;
        }
        passes.applyDiff(diff);
    }
}

} // namespace render
} // namespace bl
