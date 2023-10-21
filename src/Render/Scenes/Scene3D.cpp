#include <BLIB/Render/Scenes/Scene3D.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
namespace
{
constexpr float DefaultNear = 0.1f;
constexpr float DefaultFar  = 100.f;
} // namespace

Scene3D::Scene3D(engine::Engine& e)
: BatchedScene(e) {}

std::unique_ptr<cam::Camera> Scene3D::createDefaultCamera() {
    auto cam = std::make_unique<cam::Camera3D>();
    cam->setNearAndFarPlanes(DefaultNear, DefaultFar);
    return cam;
}

void Scene3D::setDefaultNearAndFarPlanes(cam::Camera& cam) const {
    cam.setNearAndFarPlanes(DefaultNear, -DefaultFar);
}

} // namespace scene
} // namespace rc
} // namespace bl
