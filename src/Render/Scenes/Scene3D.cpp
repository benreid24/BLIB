#include <BLIB/Render/Scenes/Scene3D.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene3DFactory.hpp>

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

rg::Strategy* strategy = nullptr;
} // namespace

Scene3D::Scene3D(engine::Engine& e)
: BatchedScene(e)
, lighting(*static_cast<dsi::Scene3DInstance*>(descriptorSets.getDescriptorSet(
      descriptorFactories.getOrCreateFactory<dsi::Scene3DFactory>()))) {
    dsi::Scene3DInstance* lightingDescriptor =
        descriptorSets.getDescriptorSet<dsi::Scene3DInstance>();
    lightingDescriptor->owner = this;
}

std::unique_ptr<cam::Camera> Scene3D::createDefaultCamera() {
    auto cam = std::make_unique<cam::Camera3D>();
    cam->setNearAndFarPlanes(DefaultNear, DefaultFar);
    return cam;
}

void Scene3D::setDefaultNearAndFarPlanes(cam::Camera& cam) const {
    cam.setNearAndFarPlanes(DefaultNear, -DefaultFar);
}

void Scene3D::onDescriptorSync() { lighting.sync(); }

void Scene3D::useRenderStrategy(rg::Strategy* ns) { strategy = ns; }

rg::Strategy* Scene3D::getRenderStrategy() { return strategy; }

} // namespace scene
} // namespace rc
} // namespace bl
