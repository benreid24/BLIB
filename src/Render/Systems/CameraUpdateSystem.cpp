#include <BLIB/Render/Systems/CameraUpdateSystem.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace sys
{
CameraUpdateSystem::CameraUpdateSystem(Renderer& r)
: renderer(r) {}

void CameraUpdateSystem::init(engine::Engine&) {}

void CameraUpdateSystem::update(std::mutex&, float dt) { renderer.updateCameras(dt); }

} // namespace sys
} // namespace render
} // namespace bl
