#include <BLIB/Render/Scenes/Scene2D.hpp>

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene2DFactory.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
namespace
{
constexpr float DefaultNear = 0.f;
constexpr float DefaultFar  = 1000.f;
} // namespace

Scene2D::Scene2D(engine::Engine& e)
: BatchedScene(e)
, lighting(static_cast<ds::Scene2DInstance*>(descriptorSets.getDescriptorSet(
      descriptorFactories.getOrCreateFactory<ds::Scene2DFactory>()))) {}

std::unique_ptr<cam::Camera> Scene2D::createDefaultCamera() {
    auto cam = std::make_unique<cam::Camera2D>(sf::FloatRect{0.f, 0.f, 1920.f, 1080.f});
    cam->setNearAndFarPlanes(DefaultNear, -DefaultFar);
    return cam;
}

void Scene2D::setDefaultNearAndFarPlanes(cam::Camera& cam) const {
    cam.setNearAndFarPlanes(DefaultNear, -DefaultFar);
}

} // namespace scene
} // namespace rc
} // namespace bl
