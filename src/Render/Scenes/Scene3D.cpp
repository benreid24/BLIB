#include <BLIB/Render/Scenes/Scene3D.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene3DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/ShadowMapFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/ShadowMapInstance.hpp>
#include <BLIB/Render/Graph/Strategies/Scene3DForwardRenderStrategy.hpp>

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

rgi::Scene3DForwardRenderStrategy defaultStrategy;
rg::Strategy* strategy = &defaultStrategy;
} // namespace

Scene3D::Scene3D(engine::Engine& e)
: BatchedScene(e)
, lighting(*static_cast<ds::Scene3DInstance*>(descriptorSets.getDescriptorSet(
      descriptorFactories.getOrCreateFactory<ds::Scene3DFactory>()))) {
    descriptorSets.getDescriptorSet<ds::Scene3DInstance>()->owner = this;

    descriptorSets.getDescriptorSet(descriptorFactories.getOrCreateFactory<ds::ShadowMapFactory>());
    descriptorSets.getDescriptorSet<ds::ShadowMapInstance>()
        ->getBinding<ds::ShadowMapBinding>()
        .setLighting(lighting);
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
