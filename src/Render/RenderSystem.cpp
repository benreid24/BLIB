#include <BLIB/Render/RenderSystem.hpp>

namespace bl
{
namespace render
{
RenderSystem::RenderSystem() {}

void RenderSystem::update(float dt) { cameraSystem.update(dt); }

Cameras& RenderSystem::cameras() { return cameraSystem; }

const Cameras& RenderSystem::cameras() const { return cameraSystem; }

} // namespace render
} // namespace bl
