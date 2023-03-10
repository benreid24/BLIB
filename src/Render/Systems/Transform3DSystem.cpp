#include <BLIB/Render/Systems/Transform3DSystem.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace render
{
namespace sys
{
void Transform3DSystem::update(std::mutex&, float) {
    pool->forEach([](t3d::Transform3D& t) {
        if (t.isDirty()) { t.computeTransform(); }
    });
}

void Transform3DSystem::init(engine::Engine& engine) {
    pool = &engine.ecs().getAllComponents<t3d::Transform3D>();
}

} // namespace sys
} // namespace render
} // namespace bl
