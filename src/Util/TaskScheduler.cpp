#include <BLIB/Util/TaskScheduler.hpp>

namespace bl
{
namespace util
{
TaskScheduler::ScheduledTask::ScheduledTask(TaskId id, ThreadPool::Task&& task, float timeUntil,
                                            float interval)
: id(id)
, task(std::move(task))
, timeUntil(timeUntil)
, interval(interval) {}

TaskScheduler::TaskScheduler()
: threadPool(nullptr)
, nextTaskId(0) {}

TaskScheduler::TaskScheduler(ThreadPool& threadPool)
: TaskScheduler() {
    this->threadPool = &threadPool;
}

TaskScheduler::TaskId TaskScheduler::scheduleTask(ThreadPool::Task&& task, float timeUntil) {
    std::unique_lock lock(mutex);

    const TaskId id = nextTaskId++;
    tasks.emplace_back(id, std::move(task), timeUntil);
    return id;
}

TaskScheduler::TaskId TaskScheduler::scheduleRepeatedTask(ThreadPool::Task&& task, float interval,
                                                          float timeUntil) {
    std::unique_lock lock(mutex);

    const TaskId id = nextTaskId++;
    timeUntil       = timeUntil >= 0.f ? timeUntil : interval;
    tasks.emplace_back(id, std::move(task), timeUntil, interval);
    return id;
}

bool TaskScheduler::cancelTask(TaskId id) {
    std::unique_lock lock(mutex);

    for (auto it = tasks.begin(); it != tasks.end(); ++it) {
        if (it->id == id) {
            tasks.erase(it);
            return true;
        }
    }
    return false;
}

void TaskScheduler::update(float dt) {
    std::unique_lock lock(mutex);
    for (auto it = tasks.begin(); it != tasks.end();) {
        it->timeUntil -= dt;
        if (it->timeUntil <= 0.f) {
            dispatch(*it);
            if (it->interval > 0.f) {
                it->timeUntil += it->interval;
                ++it;
            }
            else { it = tasks.erase(it); }
        }
        else { ++it; }
    }
}

void TaskScheduler::drain() {
    std::unique_lock lock(mutex);
    for (auto& task : tasks) { task.task(); }
    tasks.clear();
}

void TaskScheduler::dispatch(ScheduledTask& task) {
    if (threadPool) {
        if (task.interval > 0.f) {
            auto copy = task.task;
            threadPool->queueTask(std::move(copy));
        }
        else { threadPool->queueTask(std::move(task.task)); }
    }
    else { task.task(); }
}

} // namespace util
} // namespace bl
