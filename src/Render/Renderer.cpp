#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
Renderer::Renderer() {}

void Renderer::update(float dt) { cameraSystem.update(dt); }

Cameras& Renderer::cameras() { return cameraSystem; }

const Cameras& Renderer::cameras() const { return cameraSystem; }

} // namespace render
} // namespace bl
