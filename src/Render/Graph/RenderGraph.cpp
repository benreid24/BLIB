#include <BLIB/Render/Graph/RenderGraph.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/ExecutionContext.hpp>
#include <BLIB/Render/Graph/GraphAsset.hpp>

namespace bl
{
namespace rc
{
namespace rg
{

RenderGraph::RenderGraph(engine::Engine& engine, Renderer& renderer, AssetPool& pool)
: engine(engine)
, renderer(renderer)
, assets(pool)
, needsRebuild(false) {
    tasks.reserve(8);
    timeline.reserve(8);
}

void RenderGraph::execute(VkCommandBuffer commandBuffer, std::uint32_t oi, bool rt) {
    if (needsRebuild) { build(); }

    const ExecutionContext ctx(engine, renderer, commandBuffer, oi, rt);
    for (auto& step : timeline) {
        for (Task* task : step.tasks) {
            for (GraphAsset* input : task->assets.requiredInputs) {
                input->asset->prepareForInput(ctx);
            }
            for (GraphAsset* input : task->assets.optionalInputs) {
                if (input) { input->asset->prepareForInput(ctx); }
            }
            task->assets.output->asset->prepareForOutput(ctx);
            task->execute(ctx);
        }
    }
}

void RenderGraph::build() {
    needsRebuild = false;
    for (auto& task : tasks) { task->assets.init(task->assetTags); }
    assets.reset();

    std::vector<Task*> missingInputs;
    missingInputs.reserve(tasks.size());

    const auto addInputTask = [&missingInputs](Task* task) {
        if (std::find(missingInputs.rbegin(), missingInputs.rend(), task) == missingInputs.rend()) {
            missingInputs.emplace_back(task);
        }
    };

    const auto tryLinkConcreteInput =
        [this, &addInputTask](Task* task, const TaskInput& tags, GraphAsset*& input) {
            for (auto tag : tags.options) {
                input = assets.getAssetForInput(tag);
                if (input) return;
            }
            addInputTask(task);
        };

    // first link all inputs for concrete assets, mark tasks missing inputs
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
        for (auto tag : tags.options) {
            for (auto& ctask : tasks) {
                if (ctask.get() != task && !ctask->assets.output &&
                    ctask->assetTags.createdOutput == tag) {
                    asset                = assets.createAsset(tag, ctask.get());
                    ctask->assets.output = asset;
                    return true;
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

    // link task outputs that are not yet linked
    for (auto& task : tasks) {
        if (!task->assets.output) {
            for (std::string_view tag : task->assetTags.concreteOutputs) {
                GraphAsset* asset = assets.getAssetForOutput(tag, task.get());
                if (asset) {
                    task->assets.output = asset;
                    break;
                }
            }
        }
    }

    // assemble timeline
    GraphAsset* swapframe = assets.getFinalOutput();

    // determine depth and create reachable assets
    unsigned int depth = 0;
    traverse(
        [this, &depth](Task* task, unsigned int d) {
            depth = std::max(d, depth);
            task->assets.output->asset->create(engine, renderer);
            for (GraphAsset* input : task->assets.requiredInputs) {
                input->asset->create(engine, renderer);
            }
            for (GraphAsset* input : task->assets.optionalInputs) {
                if (input) { input->asset->create(engine, renderer); }
            }
        },
        swapframe);
    timeline.resize(depth + 1);

    // populate timeline
    traverse(
        [this](Task* task, unsigned int d) {
            (timeline.rbegin() + d)->tasks.emplace_back(task);
            task->onGraphInit();
        },
        swapframe);

    // shift tasks forward as much as possible. start by finding leaf inputs
    std::queue<std::pair<GraphAsset*, Task*>> toVisit;

    for (auto& step : timeline) {
        for (Task* task : step.tasks) {
            for (GraphAsset* asset : task->assets.requiredInputs) {
                if (!asset->outputtedBy) {
                    toVisit.emplace(asset, task);
                    asset->firstAvailableStep = 0;
                }
            }
            for (GraphAsset* asset : task->assets.optionalInputs) {
                if (asset && !asset->outputtedBy) {
                    toVisit.emplace(asset, task);
                    asset->firstAvailableStep = 0;
                }
            }
        }
    }

    const auto isInput = [](GraphAsset* asset, Task* task) -> bool {
        for (GraphAsset* i : task->assets.requiredInputs) {
            if (i == asset) { return true; }
        }
        for (GraphAsset* i : task->assets.optionalInputs) {
            if (i == asset) { return true; }
        }
        return false;
    };

    // traverse graph up from leafs and populate firstAvailableStep for each asset
    while (!toVisit.empty()) {
        auto node = toVisit.front();
        toVisit.pop();

        node.second->assets.output->firstAvailableStep = std::max(
            node.second->assets.output->firstAvailableStep, node.first->firstAvailableStep + 1);

        for (auto& task : tasks) {
            if (isInput(node.second->assets.output, task.get())) {
                toVisit.emplace(node.second->assets.output, task.get());
            }
        }
    }

    // for each task now determine soonest step it can run and move it
    const auto findSoonest = [](Task* task) -> unsigned int {
        unsigned int soonest = std::numeric_limits<unsigned int>::max();

        for (GraphAsset* asset : task->assets.requiredInputs) {
            soonest = soonest > asset->firstAvailableStep ? asset->firstAvailableStep : soonest;
        }
        for (GraphAsset* asset : task->assets.requiredInputs) {
            if (!asset) continue;
            soonest = soonest > asset->firstAvailableStep ? asset->firstAvailableStep : soonest;
        }

        return soonest;
    };

    unsigned int i = 0;
    for (auto& step : timeline) {
        for (auto it = step.tasks.begin(); it != step.tasks.end();) {
            Task* task                 = *it;
            const unsigned int soonest = findSoonest(task);
            if (soonest != i) {
                it = step.tasks.erase(it);
                timeline[soonest].tasks.emplace_back(task);
            }
            else { ++it; }
        }

        ++i;
    }
}

void RenderGraph::markDirty() { needsRebuild = true; }

} // namespace rg
} // namespace rc
} // namespace bl
