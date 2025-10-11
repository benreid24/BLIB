#include <BLIB/Render/Graph/RenderGraph.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/ExecutionContext.hpp>
#include <BLIB/Render/Graph/GraphAsset.hpp>
#include <BLIB/Render/Graph/Strategy.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rg
{

RenderGraph::RenderGraph(engine::Engine& engine, Renderer& renderer, AssetPool& pool,
                         RenderTarget* observer, Scene* scene)
: engine(engine)
, renderer(renderer)
, observer(observer)
, scene(scene)
, assets(renderer, pool, observer, scene)
, timeline(engine, renderer, observer, scene, &assets)
, strategy(nullptr)
, strategyVersion(0)
, needsRebuild(false)
, needsReset(true) {
    tasks.reserve(8);
}

void RenderGraph::execute(VkCommandBuffer commandBuffer, std::uint32_t oi, bool rt) {
    if (needsRebuild) { build(); }

    const ExecutionContext ctx(engine, renderer, commandBuffer, oi, rt, false);
    timeline.execute(ctx);
}

void RenderGraph::build() {
    needsRebuild = false;
    for (auto& task : tasks) { task->assets.init(task->assetTags); }
    assets.reset();

    std::vector<Task*> missingInputs;
    missingInputs.reserve(tasks.size());

    const auto addTaskMissingInput = [&missingInputs](Task* task) {
        if (std::find(missingInputs.rbegin(), missingInputs.rend(), task) == missingInputs.rend()) {
            missingInputs.emplace_back(task);
        }
    };

    const auto tryLinkConcreteInput =
        [this, &addTaskMissingInput](Task* task, const TaskInput& tags, GraphAsset*& input) {
            for (auto tag : tags.options) {
                input = assets.getAssetForInput(tag, "");
                if (input) return;
            }
            addTaskMissingInput(task);
        };

    // first link all inputs for external assets, mark tasks missing inputs
    for (auto& task : tasks) {
        for (unsigned int i = 0; i < task->assetTags.requiredInputs.size(); ++i) {
            tryLinkConcreteInput(
                task.get(), task->assetTags.requiredInputs[i], task->assets.requiredInputs[i]);
        }
        for (unsigned int i = 0; i < task->assetTags.optionalInputs.size(); ++i) {
            tryLinkConcreteInput(
                task.get(), task->assetTags.optionalInputs[i], task->assets.optionalInputs[i]);
        }
    }

    const auto findAssetCreator =
        [this](Task* task, const TaskInput& tags, GraphAsset*& asset) -> bool {
        for (auto tag : tags.options) {          // for all input slot options
            for (auto& creatorTask : tasks) {    // for all tasks
                if (creatorTask.get() != task) { // that are not the requesting task
                    for (auto& output :
                         creatorTask->assetTags.outputs) {    // for each task output slot
                        for (auto& option : output.options) { // for each output slot option
                            // if the slot matches and the task is allowed to create the asset
                            if (option.tag == tag &&
                                option.createMode == TaskOutput::CreatedByTask) {
                                const unsigned int i =
                                    &output - creatorTask->assetTags.outputs.data();
                                auto*& outputPtr = creatorTask->assets.outputs[i];

                                // create the asset if it does not exist
                                if (!outputPtr) {
                                    asset = assets.createAsset(tag, output.purpose);
                                    asset->outputtedBy.emplace_back(creatorTask.get());
                                    outputPtr = asset;
                                    return true;
                                }

                                // allow tasks to share already created assets if permitted
                                else if (tags.shareMode != TaskInput::Exclusive &&
                                         option.shareMode != TaskOutput::Exclusive &&
                                         outputPtr->asset->getTag() == tag) {
                                    outputPtr->outputtedBy.emplace_back(task);
                                    asset = outputPtr;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    };

    // now find tasks that can provide missing inputs
    for (Task* task : missingInputs) {
        for (unsigned int i = 0; i < task->assetTags.requiredInputs.size(); ++i) {
            if (task->assets.requiredInputs[i] == nullptr) {
                if (!findAssetCreator(
                        task, task->assetTags.requiredInputs[i], task->assets.requiredInputs[i])) {
                    throw std::runtime_error("Failed to find or create required input for task");
                }
            }
        }
        for (unsigned int i = 0; i < task->assetTags.optionalInputs.size(); ++i) {
            if (task->assets.optionalInputs[i] == nullptr) {
                findAssetCreator(
                    task, task->assetTags.optionalInputs[i], task->assets.optionalInputs[i]);
            }
        }
    }

    auto& missingOutputs = missingInputs;
    missingOutputs.clear();

    const auto linkRemainingOutputs = [this, &missingOutputs](Task* task) -> bool {
        bool allLinked = true;
        for (unsigned int i = 0; i < task->assets.outputs.size(); ++i) {
            if (task->assets.outputs[i]) { continue; }

            const auto& params = task->assetTags.outputs[i];
            for (auto& option : params.options) {
                if (option.shareMode != TaskOutput::Shared) { continue; }

                GraphAsset* asset = nullptr;

                // if the output is created by another task, try to find it
                if (!params.sharedWith.empty()) {
                    asset = assets.getAssetForSharedOutput(
                        option.tag, params.sharedWith, params.purpose);
                }

                // otherwise search for external assets
                else { asset = assets.getAssetForOutput(option.tag, task, params.purpose); }

                if (asset) {
                    asset->outputtedBy.emplace_back(task);
                    task->assets.outputs[i] = asset;
                    break;
                }
            }
            if (!task->assets.outputs[i]) { allLinked = false; }
        }
        return allLinked;
    };

    // link remaining task outputs to external (or other task) assets
    for (auto& task : tasks) {
        if (!linkRemainingOutputs(task.get())) { missingOutputs.emplace_back(task.get()); }
    }

    // do more passes until no more outputs can be linked
    while (!missingOutputs.empty()) {
        bool madeProgress = false;
        for (auto it = missingOutputs.begin(); it != missingOutputs.end();) {
            if (linkRemainingOutputs(*it)) {
                it           = missingOutputs.erase(it);
                madeProgress = true;
            }
            else { ++it; }
        }
        // if no progress was made, throw an error
        if (!madeProgress) { throw std::runtime_error("Failed to link all task outputs"); }
    }

    // build timeline
    timeline.build(tasks, assets.getFinalOutput());

    // notify tasks of graph creation
    for (auto& task : tasks) { task->onGraphInit(); }
}

void RenderGraph::markDirty() { needsRebuild = true; }

void RenderGraph::reset() { needsReset = true; }

void RenderGraph::populate(Scene& scene) {
    needsReset = false;
    tasks.clear();
    scene.getRenderStrategy()->populate(*this);
    strategy        = scene.getRenderStrategy();
    strategyVersion = strategy->getVersion();
    build();
}

void RenderGraph::update(float dt) {
    for (auto& task : tasks) { task->update(dt); }
}

bool RenderGraph::needsRepopulation() const {
    return needsReset || (scene && scene->getRenderStrategy() != strategy) ||
           (strategy && strategyVersion != strategy->getVersion());
}

} // namespace rg
} // namespace rc
} // namespace bl
