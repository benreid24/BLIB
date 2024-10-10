#include <BLIB/Systems/MarkedForDeath.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace sys
{
MarkedForDeath::MarkedForDeath()
: engine(nullptr)
, registry(nullptr)
, pool(nullptr) {
    toDelete.reserve(64);
}

void MarkedForDeath::init(engine::Engine& e) {
    engine   = &e;
    registry = &e.ecs();
    pool     = &registry->getAllComponents<com::MarkedForDeath>();
}

void MarkedForDeath::update(std::mutex&, float dt, float, float, float) {
    pool->forEach([this, dt](ecs::Entity ent, com::MarkedForDeath& mark) {
        if ((mark.mask & engine->getCurrentStateMask()) != 0) {
            mark.lifeLeft -= dt;
            if (mark.lifeLeft <= 0.f) { toDelete.emplace_back(ent); }
        }
    });

    for (ecs::Entity ent : toDelete) { registry->destroyEntity(ent); }
    toDelete.clear();
}

} // namespace sys
} // namespace bl