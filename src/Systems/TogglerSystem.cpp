#include <BLIB/Systems/TogglerSystem.hpp>

#include <BLIB/Engine.hpp>

namespace bl
{
namespace sys
{
void TogglerSystem::init(engine::Engine& engine) {
    pool = &engine.ecs().getAllComponents<com::Toggler>();
}

void TogglerSystem::update(std::mutex&, float dt) {
    pool->forEach([dt](ecs::Entity, com::Toggler& tog) {
        tog.time += dt;
        float period = *tog.value ? tog.onPeriod : tog.offPeriod;
        if (tog.time >= period) {
            *tog.value = !*tog.value;
            tog.time -= std::floor(tog.time / period) * period;
        }
    });
}

} // namespace sys
} // namespace bl
