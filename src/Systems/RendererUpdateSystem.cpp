#include <BLIB/Systems/RendererUpdateSystem.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace sys
{
RendererUpdateSystem::RendererUpdateSystem(rc::Renderer& r)
: renderer(r) {}

void RendererUpdateSystem::init(engine::Engine&) {}

void RendererUpdateSystem::update(std::mutex&, float dt) { renderer.update(dt); }

} // namespace sys
} // namespace bl
