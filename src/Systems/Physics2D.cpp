#include <BLIB/Systems/Physics2D.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Math.hpp>
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

Physics2D::~Physics2D() {
    if (b2World_IsValid(worldId)) { b2DestroyWorld(worldId); }
}

void Physics2D::setGravity(const glm::vec2& g) {
    gravity = g;
    b2World_SetGravity(worldId, {g.x, g.y});
}

void Physics2D::setLengthUnitScale(float s) { worldToBoxScale = s; }

bool Physics2D::addPhysicsToEntity(ecs::Entity entity, b2BodyDef bodyDef, b2ShapeDef shapeDef) {
    auto set = engine->ecs().getComponentSet<ecs::Require<com::Hitbox2D, com::Transform2D>>(entity);
    if (!set.isValid()) {
        BL_LOG_ERROR << "Failed to add physics to entity: " << entity;
        return false;
    }

    b2BodyId bodyId;
    const auto createBody = [this, &set, &bodyId, &bodyDef]() -> b2BodyId {
        const glm::vec2 pos = set.get<com::Transform2D>()->getGlobalPosition() * worldToBoxScale;
        bodyDef.position.x  = pos.x;
        bodyDef.position.y  = pos.y;
        bodyDef.rotation =
            b2MakeRot(math::degreesToRadians(set.get<com::Transform2D>()->getRotation()));
        bodyId = b2CreateBody(worldId, &bodyDef);
        return bodyId;
    };

    const glm::vec2 origin = set.get<com::Transform2D>()->getOrigin() * worldToBoxScale;
    switch (set.get<com::Hitbox2D>()->getType()) {
    case com::Hitbox2D::Circle: {
        b2Circle circle;
        circle.center.x = origin.x;
        circle.center.y = origin.y;
        circle.radius   = set.get<com::Hitbox2D>()->getRadius();
        b2CreateCircleShape(createBody(), &shapeDef, &circle);
    } break;

    case com::Hitbox2D::Rectangle: {
        const glm::vec2 hsize = set.get<com::Hitbox2D>()->getSize() * worldToBoxScale * 0.5f;
        const glm::vec2 diff  = hsize - origin;
        b2Polygon box         = b2MakeOffsetBox(hsize.x, hsize.y, {diff.x, diff.y}, 0.f);
        b2CreatePolygonShape(createBody(), &shapeDef, &box);
    } break;

    default:
        BL_LOG_ERROR << "Invalid hitbox type for physics simulation: "
                     << set.get<com::Hitbox2D>()->getType();
        return false;
    }

    engine->ecs().emplaceComponent<com::Physics2D>(
        entity, *this, entity, *set.get<com::Transform2D>(), bodyId);

    return true;
}

void Physics2D::init(engine::Engine& e) {
    engine = &e;

    b2WorldDef def = b2DefaultWorldDef();
    def.gravity.x  = gravity.x;
    def.gravity.y  = gravity.y;
    // TODO - set scale in Box2D or just scale here for every object every frame?

    def.userTaskContext = new TaskSet(e.threadPool());
    def.enqueueTask     = &parallelizePhysicsTasks;
    def.finishTask      = &waitPhysicsTasks;
    def.workerCount     = MaxWorkers;

    worldId = b2CreateWorld(&def);

    event::Dispatcher::subscribe(this);
}

void Physics2D::update(std::mutex&, float dt, float, float, float) {
    b2World_Step(worldId, dt, 4);

    b2BodyEvents moveEvents = b2World_GetBodyEvents(worldId);
    for (int i = 0; i < moveEvents.moveCount; ++i) {
        auto& event        = moveEvents.moveEvents[i];
        com::Physics2D& pc = *static_cast<com::Physics2D*>(event.userData);
        pc.transform->setPosition(event.transform.p.x / worldToBoxScale,
                                  event.transform.p.y / worldToBoxScale);
        pc.transform->setRotation(math::radiansToDegrees(b2Rot_GetAngle(event.transform.q)));
    }

    const auto getComponent = [this](b2ShapeId shape) -> com::Physics2D& {
        const b2BodyId body = b2Shape_GetBody(shape);
        return *static_cast<com::Physics2D*>(b2Body_GetUserData(body));
    };

    b2ContactEvents contacts = b2World_GetContactEvents(worldId);
    for (int i = 0; i < contacts.beginCount; ++i) {
        com::Physics2D& a = getComponent(contacts.beginEvents[i].shapeIdA);
        com::Physics2D& b = getComponent(contacts.beginEvents[i].shapeIdB);
        event::Dispatcher::dispatch<EntityCollisionBeginEvent>({a.entity, b.entity});
    }
    for (int i = 0; i < contacts.endCount; ++i) {
        com::Physics2D& a = getComponent(contacts.endEvents[i].shapeIdA);
        com::Physics2D& b = getComponent(contacts.endEvents[i].shapeIdB);
        event::Dispatcher::dispatch<EntityCollisionEndEvent>({a.entity, b.entity});
    }
    for (int i = 0; i < contacts.hitCount; ++i) {
        com::Physics2D& a = getComponent(contacts.hitEvents[i].shapeIdA);
        com::Physics2D& b = getComponent(contacts.hitEvents[i].shapeIdB);
        event::Dispatcher::dispatch<EntityCollisionHitEvent>(
            {a.entity, b.entity, contacts.hitEvents[i]});
    }
}

void Physics2D::observe(const ecs::event::ComponentRemoved<com::Physics2D>& event) {
    b2DestroyBody(event.component.bodyId);
}

} // namespace sys
} // namespace bl
