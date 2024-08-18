#include <BLIB/Particles/ParticleSystem.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace pcl
{
ParticleSystem::ParticleSystem()
: engine(nullptr)
, engineThreadpool(nullptr) {}

void ParticleSystem::removeUniqueSystem(ParticleManagerBase* system) {
    std::unique_lock lock(mutex);

    for (auto it = singleSystems.begin(); it != singleSystems.end(); ++it) {
        if (it->second.get() == system) {
            singleSystems.erase(it);
            return;
        }
    }
}

void ParticleSystem::removeUniqueSystems() {
    std::unique_lock lock(mutex);
    singleSystems.clear();
}

void ParticleSystem::removeRepeatedSystem(ParticleManagerBase* system) {
    std::unique_lock lock(mutex);

    for (auto& p : multiSystems) {
        for (auto it = p.second.begin(); it != p.second.end(); ++it) {
            if (it->get() == system) {
                p.second.erase(it);
                return;
            }
        }
    }
}

void ParticleSystem::removeAllSystems() {
    std::unique_lock lock(mutex);
    singleSystems.clear();
    multiSystems.clear();
}

void ParticleSystem::init(engine::Engine& e) {
    engine           = &e;
    engineThreadpool = &e.threadPool();
    particleThreadpool.start(4);
}

void ParticleSystem::update(std::mutex&, float dt, float realDt, float, float) {
    std::unique_lock lock(mutex);

    futures.reserve(singleSystems.size() + multiSystems.size());
    for (auto& p : singleSystems) {
        futures.emplace_back(engineThreadpool->queueTask(
            [this, &p, dt, realDt]() { p.second->update(particleThreadpool, dt, realDt); }));
    }
    for (auto& p : multiSystems) {
        for (auto& m : p.second) {
            futures.emplace_back(engineThreadpool->queueTask(
                [this, &m, dt, realDt]() { m->update(particleThreadpool, dt, realDt); }));
        }
    }
    for (auto& f : futures) { f.wait(); }
    futures.clear();
}

void ParticleSystem::earlyCleanup() {
    particleThreadpool.shutdown();
    removeAllSystems();
}

} // namespace pcl
} // namespace bl
