#include <BLIB/Render/Systems/CameraUpdateSystem.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace sys
{
CameraUpdateSystem::CameraUpdateSystem(Renderer& r)
: renderer(r) {}

void CameraUpdateSystem::init(engine::Engine&) {}

void CameraUpdateSystem::update(std::mutex&, float dt) { renderer.updateCameras(dt); }

} // namespace sys
} // namespace rc
} // namespace bl
