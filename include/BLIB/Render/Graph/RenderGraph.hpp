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
class Scene;
class Observer;

/// Collection of classes that comprise the render graph
namespace rg
{
class Strategy;

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
     * @param observer The observer the graph belongs to
     */
    RenderGraph(engine::Engine& engine, Renderer& renderer, AssetPool& assetPool,
                Observer* observer);

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
        static_cast<Task*>(task)->create(engine, renderer);
        return task;
    }

    /**
     * @brief Adds a new task to the graph to become a part of the render process. Only adds the
     *        task if it would be unique. Prevents duplicate tasks
     *
     * @tparam T The type of task to add
     * @tparam ...TArgs Argument types to the task's constructor
     * @param ...args Arguments to the task's constructor
     * @return A pointer to the new task
     */
    template<typename T, typename... TArgs>
    T* putUniqueTask(TArgs&&... args) {
        T* t = findTask<T>();
        if (t) return t;
        return putTask<T, TArgs...>(std::forward<TArgs>(args)...);
    }

    /**
     * @brief Removes the task with the given type from the graph
     *
     * @tparam T The task type to remove
     * @return True if a task was removed, false if not
     */
    template<typename T>
    bool removeTask() {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if (dynamic_cast<T*>(it->get()) != nullptr) {
                tasks.erase(it);
                needsRebuild = true;
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Removes all tasks of the given type from the graph
     *
     * @tparam T The type of task to remove
     * @return True if a task or tasks were able to be removed, false otherwise
     */
    template<typename T>
    bool removeTasks() {
        bool r = false;
        while (removeTask<T>()) { r = true; }
        return r;
    }

    /**
     * @brief Finds and returns a pointer to the first task found of the given type
     *
     * @tparam T The task to search for
     * @return A pointer to a found task, nullptr if not found
     */
    template<typename T>
    T* findTask() {
        for (auto& task : tasks) {
            T* t = dynamic_cast<T*>(task.get());
            if (t) { return t; }
        }
        return nullptr;
    }

    /**
     * @brief Returns whether or not the given task is present in the graph
     *
     * @tparam T The task type to check for
     * @return True if the task is in the graph, false otherwise
     */
    template<typename T>
    bool hasTask() {
        return findTask<T>() != nullptr;
    }

    /**
     * @brief Executes the graph to record render commands into the given command buffer. Does not
     *        execute the final task
     *
     * @param commandBuffer The command buffer to record into
     * @param observerIndex The observer's index in the current scene
     * @param renderTexture True if the final target is a render texture, false otherwise
     */
    void execute(VkCommandBuffer commandBuffer, std::uint32_t observerIndex, bool renderTexture);

    /**
     * @brief Executes the final task of the graph
     *
     * @param commandBuffer The command buffer to record into
     * @param observerIndex The observer's index in the current scene
     * @param renderTexture True if the final target is a render texture, false otherwise
     */
    void executeFinal(VkCommandBuffer commandBuffer, std::uint32_t observerIndex,
                      bool renderTexture);

    /**
     * @brief Builds the graph timeline. Called automatically in execute()
     */
    void build();

    /**
     * @brief Notifies the graph to rebuild on next execution
     */
    void markDirty();

    /**
     * @brief Returns whether or not the graph needs to be repopulated from scratch
     */
    constexpr bool needsRepopulation() const { return needsReset; }

    /**
     * @brief Clears all tasks from the graph and marks it for re-population
     */
    void reset();

    /**
     * @brief Clears and re-populates the tasks in the graph. Re-builds the graph
     *
     * @param strategy The renderer strategy to use
     * @param scene The scene to get additional tasks from
     */
    void populate(Strategy& strategy, Scene& scene);

    /**
     * @brief Calls update on all contained tasks
     *
     * @param dt The elapsed time in seconds
     */
    void update(float dt);

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
    Observer* observer;
    GraphAssetPool assets;
    std::vector<std::unique_ptr<Task>> tasks;
    std::vector<TimelineStage> timeline;
    bool needsRebuild;
    bool needsReset;

    template<typename T>
    void traverse(T&& taskVisitor, GraphAsset* start) {
        std::queue<VisitorStep> toVisit;
        std::unordered_set<GraphAsset*> visited;
        toVisit.emplace(VisitorStep{start, 0});

        const auto processInputs = [&toVisit, &visited](std::vector<GraphAsset*>& inputs,
                                                        unsigned int newSteps) {
            for (GraphAsset* asset : inputs) {
                if (!asset || !asset->outputtedBy) continue;

                if (visited.find(asset) != visited.end()) {
                    throw std::runtime_error("Cycle detected in render graph");
                }
                visited.emplace(asset);
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
