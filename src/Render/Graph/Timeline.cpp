#include <BLIB/Render/Graph/Timeline.hpp>

#include <BLIB/Events.hpp>
#include <BLIB/Render/Events/GraphEvents.hpp>
#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Graph/ExecutionContext.hpp>
#include <BLIB/Render/Graph/Task.hpp>
#include <limits>
#include <queue>
#include <unordered_map>

namespace bl
{
namespace rc
{
namespace rg
{
namespace
{
constexpr unsigned int UnsetStep = std::numeric_limits<unsigned int>::max();

struct VisitorStep {
    Task* task;
    unsigned int step;
};

} // namespace

Timeline::TaskGroup::TaskGroup(GraphAsset* output)
: output(output) {
    tasks.reserve(4);
}

Timeline::TimelineStage::TimelineStage() { taskGroups.reserve(4); }

Timeline::Timeline(engine::Engine& engine, Renderer& renderer, RenderTarget* target, Scene* scene)
: engine(engine)
, renderer(renderer)
, observer(target)
, scene(scene) {
    timeline.reserve(4);
}

void Timeline::execute(const ExecutionContext& ctx) {
    for (auto& stage : timeline) { stage.execute(ctx); }
}

void Timeline::TimelineStage::execute(const ExecutionContext& ctx) {
    // prepare all inputs first
    for (TaskGroup& group : taskGroups) {
        for (auto& task : group.tasks) { task.first->prepareInputs(ctx); }
    }

    // then execute by group
    for (TaskGroup& group : taskGroups) { group.execute(ctx); }
}

void Timeline::TaskGroup::execute(const ExecutionContext& ctx) {
    output->asset->startOutput(ctx);
    for (auto& task : tasks) { task.first->execute(ctx, &output->asset.get()); }
    output->asset->endOutput(ctx);
}

void Timeline::build(std::vector<std::unique_ptr<Task>>& tasks, GraphAsset* finalAsset) {
    timeline.clear();

    const auto resetAssetIndices = [this](Task* task) {
        for (GraphAsset* output : task->assets.outputs) { output->firstAvailableStep = UnsetStep; }
        for (GraphAsset* asset : task->assets.requiredInputs) {
            asset->firstAvailableStep = UnsetStep;
        }
        for (GraphAsset* asset : task->assets.optionalInputs) {
            if (asset) { asset->firstAvailableStep = UnsetStep; }
        }
    };

    const auto taskIsFirst = [this](Task* task) -> bool {
        for (GraphAsset* asset : task->assets.requiredInputs) {
            if (!asset->outputtedBy.empty()) { return false; }
        }
        for (GraphAsset* asset : task->assets.optionalInputs) {
            if (asset && !asset->outputtedBy.empty()) { return false; }
        }
        return true;
    };

    const auto isInput = [](GraphAsset* asset, Task* task) -> bool {
        for (GraphAsset* i : task->assets.requiredInputs) {
            if (i == asset) { return true; }
        }
        for (GraphAsset* i : task->assets.optionalInputs) {
            if (i == asset) { return true; }
        }
        return false;
    };

    const auto createAssets = [this](Task* task) {
        for (GraphAsset* asset : task->assets.requiredInputs) {
            if (asset->asset->create(engine, renderer, observer)) {
                bl::event::Dispatcher::dispatch<event::SceneGraphAssetInitialized>(
                    {.target = observer, .scene = scene, .asset = asset});
            }
        }
        for (GraphAsset* asset : task->assets.optionalInputs) {
            if (asset) {
                if (asset->asset->create(engine, renderer, observer)) {
                    bl::event::Dispatcher::dispatch<event::SceneGraphAssetInitialized>(
                        {.target = observer, .scene = scene, .asset = asset});
                }
            }
        }
        for (GraphAsset* asset : task->assets.outputs) {
            if (asset->asset->create(engine, renderer, observer)) {
                bl::event::Dispatcher::dispatch<event::SceneGraphAssetInitialized>(
                    {.target = observer, .scene = scene, .asset = asset});
            }
        }
    };

    // find all tasks that can run in first step & reset asset indices
    std::vector<VisitorStep> tasksToVisit;
    tasksToVisit.reserve(tasks.size() * 2);
    for (auto& task : tasks) {
        resetAssetIndices(task.get());
        if (taskIsFirst(task.get())) { tasksToVisit.emplace_back(task.get(), 0); }
    }
    if (finalAsset->firstAvailableStep != UnsetStep) {
        throw std::runtime_error("RenderGraph does not output final asset");
    }

    // traverse up graph from starting tasks and assign step indices to assets
    unsigned int maxSteps = 0;
    while (!tasksToVisit.empty()) {
        VisitorStep current = tasksToVisit.back();
        tasksToVisit.pop_back();

        createAssets(current.task);
        maxSteps                    = std::max(maxSteps, current.step);
        const unsigned int nextStep = current.step + 1;

        for (GraphAsset* output : current.task->assets.outputs) {
            if (output->firstAvailableStep == UnsetStep ||
                output->firstAvailableStep < current.step) {
                output->firstAvailableStep = current.step;

                for (auto& task : tasks) {
                    if (isInput(output, task.get())) {
                        tasksToVisit.emplace_back(task.get(), nextStep);
                    }
                }
            }
            output->firstAvailableStep = std::max(output->firstAvailableStep, current.step);
        }
    }

    if (finalAsset->firstAvailableStep == UnsetStep) {
        throw std::runtime_error("RenderGraph timeline does not reach final asset");
    }

    // create timeline and add tasks
    timeline.resize(maxSteps + 1);
    for (auto& task : tasks) {
        unsigned int i = 0;
        for (GraphAsset* output : task->assets.outputs) {
            TaskGroup* group = timeline[output->firstAvailableStep].getGroupForAsset(output);
            group->addTask(task.get(), task->assetTags.outputs[i++].order);
        }
    }
}

Timeline::TaskGroup* Timeline::TimelineStage::getGroupForAsset(GraphAsset* asset) {
    for (auto& group : taskGroups) {
        if (group.output == asset) { return &group; }
    }
    return &taskGroups.emplace_back(asset);
}

void Timeline::TaskGroup::addTask(Task* task, TaskOutput::Order order) {
    for (auto it = tasks.begin(); it != tasks.end(); ++it) {
        if (it->second >= order) {
            tasks.emplace(it, task, order);
            return;
        }
    }
    tasks.emplace_back(task, order);
}

} // namespace rg
} // namespace rc
} // namespace bl
