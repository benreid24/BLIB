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
bool worldOverlapCallback(b2ShapeId shape, void* ctx) {
    b2ShapeId* dst = static_cast<b2ShapeId*>(ctx);
    *dst           = shape;
    return false; // true = keep searching, false = found
}
} // namespace

Physics2D::Physics2D()
: engine(nullptr) {
    worlds.resize(engine::World::MaxWorlds, nullptr);
}

com::Physics2D* Physics2D::addPhysicsToEntity(ecs::Entity entity, b2BodyDef bodyDef,
                                              b2ShapeDef shapeDef) {
    auto set = engine->ecs().getComponentSet<ecs::Require<com::Hitbox2D, com::Transform2D>>(entity);
    const auto bodyId = createBody(bodyDef, shapeDef, set);
    if (!bodyId.has_value()) { return nullptr; }

    return engine->ecs().emplaceComponent<com::Physics2D>(
        entity, *this, entity, *set.get<com::Transform2D>(), bodyId.value());
}

com::Physics2D* Physics2D::findEntityAtPosition(const engine::World2D& world, const glm::vec2& pos,
                                                b2QueryFilter filter) const {
    const float s = world.getWorldToBoxScale();
    const b2Circle circle{.center = {s, s}, .radius = s};
    b2Transform transform = b2Transform_identity;
    transform.p           = {pos.x * s, pos.y * s};
    b2ShapeId found       = b2_nullShapeId;
    b2World_OverlapCircle(
        world.getBox2dWorldId(), &circle, transform, filter, &worldOverlapCallback, &found);
    if (B2_IS_NULL(found)) { return nullptr; }
    const b2BodyId body = b2Shape_GetBody(found);
    return static_cast<com::Physics2D*>(b2Body_GetUserData(body));
}

void Physics2D::createSensorForEntity(ecs::Entity entity, b2Filter filter) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type      = b2_staticBody;

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.isSensor   = true;
    shapeDef.filter     = filter;

    auto set = engine->ecs().getComponentSet<ecs::Require<com::Hitbox2D, com::Transform2D>>(entity);
    const auto bodyId = createBody(bodyDef, shapeDef, set);
    if (!bodyId.has_value()) { return; }

    engine->ecs().emplaceComponent<com::Physics2D>(
        entity, *this, entity, *set.get<com::Transform2D>(), bodyId.value());
}

void Physics2D::init(engine::Engine& e) {
    engine = &e;
    event::Dispatcher::subscribe(this);
}

void Physics2D::update(std::mutex&, float dt, float, float, float) {
    for (auto world : worlds) {
        if (!world) { continue; }

        const b2WorldId worldId     = world->getBox2dWorldId();
        const float worldToBoxScale = world->getWorldToBoxScale();

        b2World_Step(worldId, dt, 4);

        b2BodyEvents moveEvents = b2World_GetBodyEvents(worldId);
        for (int i = 0; i < moveEvents.moveCount; ++i) {
            auto& event        = moveEvents.moveEvents[i];
            com::Physics2D& pc = *static_cast<com::Physics2D*>(event.userData);
            pc.transform->setPosition(event.transform.p.x / worldToBoxScale,
                                      event.transform.p.y / worldToBoxScale);
            if (!b2Body_IsFixedRotation(event.bodyId)) {
                pc.transform->setRotation(
                    math::radiansToDegrees(b2Rot_GetAngle(event.transform.q)));
            }
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

        b2SensorEvents sensors = b2World_GetSensorEvents(worldId);
        for (int i = 0; i < sensors.beginCount; ++i) {
            com::Physics2D& entity = getComponent(sensors.beginEvents[i].visitorShapeId);
            com::Physics2D& sensor = getComponent(sensors.beginEvents[i].sensorShapeId);
            event::Dispatcher::dispatch<SensorEntered>({entity.entity, sensor.entity});
        }
        for (int i = 0; i < sensors.endCount; ++i) {
            com::Physics2D& entity = getComponent(sensors.endEvents[i].visitorShapeId);
            com::Physics2D& sensor = getComponent(sensors.endEvents[i].sensorShapeId);
            event::Dispatcher::dispatch<SensorExited>({entity.entity, sensor.entity});
        }
    }
}

com::Physics2D* Physics2D::getPhysicsComponentFromShape(b2ShapeId shapeId) {
    const b2BodyId body = b2Shape_GetBody(shapeId);
    return static_cast<com::Physics2D*>(b2Body_GetUserData(body));
}

ecs::Entity Physics2D::getEntityFromShape(b2ShapeId shapeId) {
    com::Physics2D* phys = getPhysicsComponentFromShape(shapeId);
    return phys ? phys->entity : ecs::InvalidEntity;
}

float Physics2D::getWorldToBoxScale(unsigned int wi) const {
    engine::World2D* world = worlds[wi];
    return world ? world->getWorldToBoxScale() : 1.f;
}

float Physics2D::getWorldToBoxScale(ecs::Entity ent) const {
    return getWorldToBoxScale(ent.getWorldIndex());
}

void Physics2D::observe(const ecs::event::ComponentRemoved<com::Physics2D>& event) {
    const_cast<com::Physics2D&>(event.component).system = nullptr;
    b2DestroyBody(event.component.bodyId);
}

void Physics2D::observe(const engine::event::WorldCreated& event) {
    engine::World2D* world = dynamic_cast<engine::World2D*>(&event.world);
    if (world) { worlds[world->worldIndex()] = world; }
}

void Physics2D::observe(const engine::event::WorldDestroyed& event) {
    worlds[event.world.worldIndex()] = nullptr;
}

std::optional<b2BodyId> Physics2D::createBody(
    b2BodyDef& bodyDef, b2ShapeDef& shapeDef,
    ecs::ComponentSet<ecs::Require<com::Hitbox2D, com::Transform2D>>& set) {
    if (!set.isValid()) {
        BL_LOG_ERROR << "Failed to add physics to entity (missing components): " << set.entity();
        return {};
    }

    engine::World2D* world = worlds[set.entity().getWorldIndex()];
    if (!world) {
        BL_LOG_ERROR << "Failed to add physics to entity (wrong world type): " << set.entity();
        return {};
    }

    const b2WorldId worldId     = world->getBox2dWorldId();
    const float worldToBoxScale = world->getWorldToBoxScale();

    b2BodyId bodyId;
    const auto createBody =
        [this, worldId, worldToBoxScale, &set, &bodyId, &bodyDef]() -> b2BodyId {
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
        circle.radius   = set.get<com::Hitbox2D>()->getRadius() * worldToBoxScale;
        circle.center.x = circle.radius - origin.x;
        circle.center.y = circle.radius - origin.y;
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
        return {};
    }

    return bodyId;
}

} // namespace sys
} // namespace bl
