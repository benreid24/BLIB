#include <BLIB/Systems/Physics2D.hpp>

#include <BLIB/Engine.hpp>
#include <thread>
#include <vector>

namespace bl
{
namespace sys
{
namespace
{
const unsigned int MaxWorkers =
    std::thread::hardware_concurrency() / 2 + std::thread::hardware_concurrency() % 2;

struct TaskSet {
    TaskSet(util::ThreadPool& threadPool)
    : threadPool(threadPool) {}

    util::ThreadPool& threadPool;
    std::vector<std::future<void>> tasks;
};

void* parallelizePhysicsTasks(b2TaskCallback* task, int32_t itemCount, int32_t minRange,
                              void* taskContext, void* userContext) {
    TaskSet& manager = *static_cast<TaskSet*>(userContext);
    manager.tasks.reserve(std::max(itemCount / minRange, 1));
    for (int32_t i = 0; i < itemCount; i += minRange) {
        manager.tasks.emplace_back(
            manager.threadPool.queueTask([task, itemCount, minRange, taskContext, i]() {
                // TODO - does worker index matter?
                task(i,
                     i + std::min(i + minRange, itemCount),
                     (i / minRange) % MaxWorkers,
                     taskContext);
            }));
    }

    return &manager;
}

void waitPhysicsTasks(void*, void* userContext) {
    TaskSet& manager = *static_cast<TaskSet*>(userContext);
    for (auto& f : manager.tasks) { f.wait(); }
    manager.tasks.clear();
}

} // namespace

Physics2D::Physics2D()
: worldToBoxScale(1.f)
, gravity(0.f, 0.f) {}

Physics2D::~Physics2D() { b2DestroyWorld(worldId); }

void Physics2D::setGravity(const glm::vec2& g) {
    gravity = g;
    b2World_SetGravity(worldId, {g.x, g.y});
}

void Physics2D::setLengthUnitScale(float s) { worldToBoxScale = s; }

void Physics2D::init(engine::Engine& engine) {
    b2WorldDef def = b2DefaultWorldDef();
    def.gravity.x  = gravity.x;
    def.gravity.y  = gravity.y;
    // TODO - set scale in Box2D or just scale here for every object every frame?

    def.userTaskContext = new TaskSet(engine.threadPool());
    def.enqueueTask     = &parallelizePhysicsTasks;
    def.finishTask      = &waitPhysicsTasks;
    def.workerCount     = MaxWorkers;

    worldId = b2CreateWorld(&def);
}

void Physics2D::update(std::mutex&, float dt, float, float, float) {
    b2World_Step(worldId, dt, 4);
    // TODO - read move events and update transform positions
    // TODO - read collision events and dispatch them
}

} // namespace sys
} // namespace bl
