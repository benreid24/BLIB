#include <BLIB/Systems/CameraUpdateSystem.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace sys
{
CameraUpdateSystem::CameraUpdateSystem(rc::Renderer& r)
: renderer(r) {}

void CameraUpdateSystem::init(engine::Engine&) {}

void CameraUpdateSystem::update(std::mutex&, float dt) { renderer.updateCameras(dt); }

} // namespace sys
} // namespace bl
