#include <BLIB/Render/Renderer/Scene.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderables/Renderable.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
Scene::Scene(Renderer& r)
: renderer(r)
, objects(container::ObjectPool<SceneObject>::GrowthPolicy::ExpandBuffer, 256)
, primaryStage(r) {
    toRemove.reserve(32);
    stageBatches[Config::Stage::PrimaryOpaque]      = &primaryStage.opaqueObjects;
    stageBatches[Config::Stage::PrimaryTransparent] = &primaryStage.transparentObjects;
}

SceneObject::Handle Scene::createAndAddObject(Renderable* owner) {
    std::unique_lock lock(mutex);
    return objects.getStableRef(objects.emplace(owner));
}

void Scene::removeObject(const SceneObject::Handle& obj) {
    std::unique_lock lock(eraseMutex);
    toRemove.emplace_back(obj);
}

void Scene::recordRenderCommands(const SceneRenderContext& ctx) {
    std::unique_lock lock(mutex);

    primaryStage.recordRenderCommands(ctx);
}

void Scene::sync() {
    std::unique_lock lock(mutex);
    performRemovals();

    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (it->flags.isDirty()) {
            SceneObject& object = *it;
            if (object.flags.isRenderPassDirty()) {
                SceneObject::Handle handle = objects.getStableRef(it);
                updateStageMembership(handle);
            }
            if (object.flags.isPCDirty()) { object.owner->syncPC(); }
            if (object.flags.isDrawParamsDirty()) { object.owner->syncDrawParams(); }
            object.flags.reset();
        }
    }
}

void Scene::performRemovals() {
    std::unique_lock lock(eraseMutex);
    for (SceneObject::Handle& obj : toRemove) {
        for (std::uint32_t stage = 0; stage < Config::Stage::Count; ++stage) {
            const std::uint32_t pid = obj->owner->stageMembership.getPipelineForRenderStage(stage);
            if (pid != Config::PipelineIds::None) { stageBatches[stage]->removeObject(obj, pid); }
        }
        obj.erase();
    }
    toRemove.clear();
}

void Scene::updateStageMembership(SceneObject::Handle& obj) {
    auto& stages = obj->owner->stageMembership;
    while (stages.hasDiff()) {
        const auto diff = stages.nextDiff();
        switch (diff.type) {
        case StagePipelines::Diff::Add:
            stageBatches[diff.renderStageId]->addObject(obj, diff.pipelineId);
            break;
        case StagePipelines::Diff::Edit:
            stageBatches[diff.renderStageId]->changePipeline(
                obj, stages.getPipelineForRenderStage(diff.renderStageId), diff.pipelineId);
            break;
        case StagePipelines::Diff::Remove:
            stageBatches[diff.renderStageId]->removeObject(obj, diff.pipelineId);
            break;
        }
        stages.applyDiff(diff);
    }
}

} // namespace render
} // namespace bl
