#include <BLIB/Engine/Worlds/World2D.hpp>

#include <BLIB/Systems/Physics2D.hpp>

namespace bl
{
namespace engine
{
namespace
{
struct QueryContext {
    World2D::WorldQueryCallback callback;
    void* ctx;
};

bool worldQueryCallback(b2ShapeId shape, void* ctx) {
    QueryContext* myCtx       = static_cast<QueryContext*>(ctx);
    com::Physics2D* component = sys::Physics2D::getPhysicsComponentFromShape(shape);
    return myCtx->callback(component, myCtx->ctx);
}

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

World2D::World2D(Engine& owner)
: BasicWorld(owner)
, worldToBoxScale(1.f)
, gravity(0.f, 0.f) {
    b2WorldDef def = b2DefaultWorldDef();
    def.gravity.x  = gravity.x;
    def.gravity.y  = gravity.y;

    def.userTaskContext = new TaskSet(owner.threadPool());
    def.enqueueTask     = &parallelizePhysicsTasks;
    def.finishTask      = &waitPhysicsTasks;
    def.workerCount     = MaxWorkers;

    boxWorld = b2CreateWorld(&def);
}

World2D::~World2D() {
    if (b2World_IsValid(boxWorld)) { b2DestroyWorld(boxWorld); }
}

void World2D::setGravity(const glm::vec2& g) {
    gravity = g;
    b2World_SetGravity(boxWorld, {g.x, g.y});
}

void World2D::setLengthUnitScale(float s) { worldToBoxScale = s; }

void World2D::queryAABB(glm::vec2 lower, glm::vec2 upper, b2QueryFilter filter,
                        WorldQueryCallback cb, void* ctx) const {
    QueryContext myCtx{cb, ctx};
    b2World_OverlapAABB(boxWorld,
                        {{lower.x * worldToBoxScale, lower.y * worldToBoxScale},
                         {upper.x * worldToBoxScale, upper.y * worldToBoxScale}},
                        filter,
                        &worldQueryCallback,
                        &myCtx);
}

} // namespace engine
} // namespace bl
