#include <BLIB/Systems/Physics2D.hpp>

#include <BLIB/Engine.hpp>
#include <BLIB/Math.hpp>
#include <thread>
#include <vector>

namespace bl
{
namespace sys
{
Physics2D::Physics2D()
: engine(nullptr) {
    worlds.resize(engine::World::MaxWorlds, nullptr);
}

com::Physics2D* Physics2D::addPhysicsToEntity(ecs::Entity entity, b2BodyDef bodyDef,
                                              b2ShapeDef shapeDef) {
    auto set = engine->ecs().getComponentSet<ecs::Require<com::Hitbox2D, com::Transform2D>>(entity);
    if (!set.isValid()) {
        BL_LOG_ERROR << "Failed to add physics to entity (missing components): " << entity;
        return nullptr;
    }

    engine::World2D* world = worlds[entity.getWorldIndex()];
    if (!world) {
        BL_LOG_ERROR << "Failed to add physics to entity (wrong world type): " << entity;
        return nullptr;
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
        return nullptr;
    }

    return engine->ecs().emplaceComponent<com::Physics2D>(
        entity, *this, entity, *set.get<com::Transform2D>(), bodyId);
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
}

float Physics2D::getWorldToBoxScale(unsigned int wi) const {
    engine::World2D* world = worlds[wi];
    return world ? world->getWorldToBoxScale() : 1.f;
}

float Physics2D::getWorldToBoxScale(ecs::Entity ent) const {
    return getWorldToBoxScale(ent.getWorldIndex());
}

void Physics2D::observe(const ecs::event::ComponentRemoved<com::Physics2D>& event) {
    b2DestroyBody(event.component.bodyId);
}

void Physics2D::observe(const engine::event::WorldCreated& event) {
    engine::World2D* world = dynamic_cast<engine::World2D*>(&event.world);
    if (world) { worlds[world->worldIndex()] = world; }
}

void Physics2D::observe(const engine::event::WorldDestroyed& event) {
    worlds[event.world.worldIndex()] = nullptr;
}

} // namespace sys
} // namespace bl
