#include <BLIB/Systems/Transform2DDescriptorSystem.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace sys
{
Transform2DDescriptorSystem::Transform2DDescriptorSystem()
: pool(nullptr) {}

void Transform2DDescriptorSystem::update(std::mutex&, float, float, float, float) {
    pool->forEach([](ecs::Entity, com::Transform2D& t) {
        if (t.requiresRefresh()) { t.refresh(); }
    });
}

void Transform2DDescriptorSystem::init(engine::Engine& e) {
    pool = &e.ecs().getAllComponents<com::Transform2D>();
}

} // namespace sys
} // namespace bl
