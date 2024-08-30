#include <BLIB/Systems/Physics2D.hpp>

#include <BLIB/Engine.hpp>

namespace bl
{
namespace sys
{
Physics2D::Physics2D()
: worldToBoxScale(1.f)
, gravity(0.f, 0.f) {}

Physics2D::~Physics2D() { b2DestroyWorld(worldId); }

void Physics2D::setGravity(const glm::vec2& g) {
    gravity = g;
    b2World_SetGravity(worldId, {g.x, g.y});
}

void Physics2D::setLengthUnitScale(float s) { worldToBoxScale = s; }

void Physics2D::init(engine::Engine&) {
    b2WorldDef def = b2DefaultWorldDef();
    def.gravity.x  = gravity.x;
    def.gravity.y  = gravity.y;
    // TODO - set scale in Box2D or just scale here for every object every frame?
    worldId = b2CreateWorld(&def);
}

void Physics2D::update(std::mutex&, float dt, float, float, float) {
    b2World_Step(worldId, dt, 4);
    // TODO - read move events and update transform positions
    // TODO - read collision events and dispatch them
}

} // namespace sys
} // namespace bl
