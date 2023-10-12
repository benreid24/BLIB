#include <BLIB/Systems/VelocitySystem.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace sys
{
VelocitySystem::VelocitySystem()
: view2d(nullptr) {}

void VelocitySystem::update(std::mutex&, float dt, float, float, float) {
    view2d->forEach([dt](Tags2D::TComponentSet& cset) {
        cset.get<com::Velocity2D>()->apply(*cset.get<com::Transform2D>(), dt);
    });
}

void VelocitySystem::init(engine::Engine& e) { view2d = Tags2D::getView(e.ecs()); }

} // namespace sys
} // namespace bl
