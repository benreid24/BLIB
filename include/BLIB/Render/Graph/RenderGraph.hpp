#ifndef BLIB_RENDER_RENDERGRAPH_HPP
#define BLIB_RENDER_RENDERGRAPH_HPP

#include <BLIB/Render/Graph/AssetPool.hpp>
#include <BLIB/Render/Graph/GraphAssetPool.hpp>
#include <BLIB/Render/Graph/Task.hpp>
#include <memory>
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace bl
{
namespace rc
{
/// Collection of classes that comprise the render graph
namespace rg
{
/**
 * @brief The core render graph functionality. Drives rendering for each observer
 *
 * @ingroup Renderer
 */
class RenderGraph {
public:
    /**
     * @brief Creates a new RenderGraph
     *
     * @param engine The engine instance
     * @param renderer The renderer instance
     * @param assetPool The observer's asset pool
     */
    RenderGraph(engine::Engine& engine, Renderer& renderer, AssetPool& assetPool);

    /**
     * @brief Adds a new task to the graph to become a part of the render process
     *
     * @tparam T The type of task to add
     * @tparam ...TArgs Argument types to the task's constructor
     * @param ...args Arguments to the task's constructor
     * @return A pointer to the new task
     */
    template<typename T, typename... TArgs>
    T* putTask(TArgs&&... args) {
        T* task = new T(std::forward<TArgs>(args)...);
        tasks.emplace_back(task);
        needsRebuild = true;
        return task;
    }

    /**
     * @brief Executes the graph to record render commands into the given command buffer
     *
     * @param commandBuffer The command buffer to record into
     * @param observerIndex The observer's index in the current scene
     * @param renderTexture True if the final target is a render texture, false otherwise
     */
    void execute(VkCommandBuffer commandBuffer, std::uint32_t observerIndex, bool renderTexture);

    /**
     * @brief Builds the graph timeline. Called automatically in execute()
     */
    void build();

private:
    struct TimelineStage {
        std::vector<Task*> tasks;
    };

    struct VisitorStep {
        GraphAsset* asset;
        unsigned int stepsFromEnd;
    };

    engine::Engine& engine;
    Renderer& renderer;
    GraphAssetPool assets;
    std::vector<std::unique_ptr<Task>> tasks;
    std::vector<TimelineStage> timeline;
    bool needsRebuild;

    template<typename T>
    void traverse(T&& taskVisitor, GraphAsset* start) {
        std::queue<VisitorStep> toVisit;
        toVisit.emplace(VisitorStep{start, 0});

#ifdef BLIB_DEBUG
        std::unordered_set<GraphAsset*> visited;
#endif

        const auto processInputs = [&toVisit, &visited](std::vector<GraphAsset*>& inputs,
                                                        unsigned int newSteps) {
            for (GraphAsset* asset : inputs) {
                if (!asset || !asset->outputtedBy) continue;

#ifdef BLIB_DEBUG
                if (visited.find(asset) != visited.end()) {
                    throw std::runtime_error("Cycle detected in render graph");
                }
                visited.emplace(asset);
#endif
                toVisit.emplace(VisitorStep{asset, newSteps});
            }
        };

        while (!toVisit.empty()) {
            const VisitorStep step = toVisit.front();
            toVisit.pop();

            const unsigned int newSteps = step.stepsFromEnd + 1;
            taskVisitor(step.asset->outputtedBy, step.stepsFromEnd);

            processInputs(step.asset->outputtedBy->assets.requiredInputs, newSteps);
            processInputs(step.asset->outputtedBy->assets.optionalInputs, newSteps);
        }
    }
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
