#include <BLIB/Engine/Systems.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace engine
{
Systems::StageSet::StageSet() { systems.reserve(8); };

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

    FrameStage::V i = startStage;
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
        i = static_cast<FrameStage::V>(static_cast<unsigned int>(i) + 1);
    }
}

void Systems::addFrameTask(FrameStage::V stage, Task&& task) {
    std::unique_lock lock(systems[stage].taskMutex);
    systems[stage].tasks.emplace_back(std::forward<Task>(task));
}

void Systems::StageSet::drainTasks() {
    std::unique_lock lock(taskMutex);
    for (const auto& task : tasks) { task(); }
    tasks.clear();
}

} // namespace engine
} // namespace bl
