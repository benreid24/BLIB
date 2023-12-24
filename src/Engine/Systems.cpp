#include <BLIB/Engine/Systems.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace engine
{
Systems::StageSet::StageSet()
: version(0) {
    systems.reserve(8);
};

Systems::Systems(Engine& e)
: engine(e) {}

void Systems::init() {
    for (auto& set : systems) {
        for (auto& system : set.systems) { system.system->init(engine); }
    }
}

void Systems::update(FrameStage::V startStage, FrameStage::V endStage, StateMask::V stateMask,
                     float dt, float realDt, float lag, float realLag) {
    const auto beg = systems.begin() + startStage;
    const auto end = systems.begin() + endStage;
    auto& tp       = engine.threadPool();

    for (auto it = beg; it != end; ++it) {
        if (!it->tasks.empty()) {
            tp.queueTask([it]() { it->drainTasks(); });
        }
        if (it->systems.size() > 1) {
            for (auto& system : it->systems) {
                if ((system.mask & stateMask) != 0) {
                    tp.queueTask([&system, it, dt, realDt, lag, realLag]() {
                        system.system->update(it->mutex, dt, realDt, lag, realLag);
                    });
                }
            }
        }
        else if (!it->systems.empty()) {
            auto& system = it->systems.front();
            if ((system.mask & stateMask) != 0) {
                system.system->update(it->mutex, dt, realDt, lag, realLag);
            }
        }
        tp.drain();
    }
}

void Systems::cleanup() {
    for (auto& set : systems) {
        for (auto& system : set.systems) { system.system->cleanup(); }
    }
}

Systems::TaskHandle Systems::addFrameTask(FrameStage::V stage, Task&& task) {
    auto& set = systems[stage];
    std::unique_lock lock(set.taskMutex);
    set.tasks.emplace_back(std::forward<Task>(task));
    return TaskHandle(&set, set.tasks.size() - 1, std::move(set.tasks.back().task.get_future()));
}

void Systems::StageSet::drainTasks() {
    std::unique_lock lock(taskMutex);
    for (auto& task : tasks) { task.execute(); }
    tasks.clear();
    ++version;
}

Systems::TaskHandle::TaskHandle()
: owner(nullptr) {}

Systems::TaskHandle::TaskHandle(const TaskHandle& handle)
: owner(handle.owner)
, index(handle.index)
, version(handle.version)
, future(std::move(const_cast<TaskHandle&>(handle).future)) {}

Systems::TaskHandle& Systems::TaskHandle::operator=(const TaskHandle& handle) {
    owner   = handle.owner;
    index   = handle.index;
    version = handle.version;
    future  = std::move(const_cast<TaskHandle&>(handle).future);
    return *this;
}

Systems::TaskHandle::TaskHandle(StageSet* owner, std::size_t index, std::future<void>&& future)
: owner(owner)
, index(index)
, version(owner->version)
, future(std::forward<std::future<void>>(future)) {}

bool Systems::TaskHandle::isValid() const { return future.valid(); }

bool Systems::TaskHandle::isQueued() const {
    if (owner) {
        std::unique_lock lock(owner->taskMutex);
        return version == owner->version && index < owner->tasks.size();
    }
    return false;
}

void Systems::TaskHandle::cancel() {
    if (owner) {
        std::unique_lock lock(owner->taskMutex);
        if (version == owner->version && index < owner->tasks.size()) {
            owner->tasks[index].cancelled = true;
        }
    }
}

void Systems::TaskHandle::wait() {
    if (future.valid()) { future.wait(); }
}

} // namespace engine
} // namespace bl
