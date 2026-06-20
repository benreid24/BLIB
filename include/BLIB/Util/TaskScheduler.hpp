#ifndef BLIB_UTIL_TASKSCHEDULER_HPP
#define BLIB_UTIL_TASKSCHEDULER_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Util/ThreadPool.hpp>
#include <functional>
#include <list>
#include <mutex>

namespace bl
{
namespace util
{
/**
 * @brief Lightweight scheduler for delayed and repeated tasks. Optional threadpool integration
 *
 * @ingroup Util
 */
class TaskScheduler : private NonCopyable {
public:
    /// Id for a task
    using TaskId = std::size_t;

    /**
     * @brief Creates the scheduler in immediate mode. Tasks are executed directly
     */
    TaskScheduler();

    /**
     * @brief Creates the scheduler in thread pool mode. Tasks are sent to the pool for execution
     *
     * @param threadPool The thread pool to dispatch tasks to
     */
    TaskScheduler(ThreadPool& threadPool);

    /**
     * @brief Schedules a task to be executed after a specified delay.
     *
     * @param task The task to be scheduled
     * @param timeUntil The delay in seconds before the task should be executed
     * @return A unique identifier for the scheduled task
     */
    TaskId scheduleTask(ThreadPool::Task&& task, float timeUntil);

    /**
     * @brief Schedules a task to be executed after a specified delay. Repeats on an interval until
     *        cancelled or when the scheduler is drained
     *
     * @param task The task to be scheduled
     * @param interval The interval in seconds between repeated executions of the task
     * @param timeUntil The delay in seconds before the first execution of the task
     * @return A unique identifier for the scheduled task
     */
    TaskId scheduleRepeatedTask(ThreadPool::Task&& task, float interval, float timeUntil = -1.f);

    /**
     * @brief Cancels the task with the given id if it is still pending
     *
     * @param id The id of the task to cancel and remove
     * @return True if a task was cancelled, false otherwise
     */
    bool cancelTask(TaskId id);

    /**
     * @brief Advances the scheduler and executes tasks that are ready
     *
     * @param dt Elapsed time in seconds
     */
    void update(float dt);

    /**
     * @brief Executes all pending tasks immediately on the calling thread and then removes them
     */
    void drain();

private:
    struct ScheduledTask {
        TaskId id;
        ThreadPool::Task task;
        float timeUntil;
        float interval;

        ScheduledTask(TaskId id, ThreadPool::Task&& task, float timeUntil, float interval = -1.f);
    };

    std::mutex mutex;
    ThreadPool* threadPool;
    std::list<ScheduledTask> tasks;
    std::size_t nextTaskId;

    void dispatch(ScheduledTask& task);
};

} // namespace util
} // namespace bl

#endif
