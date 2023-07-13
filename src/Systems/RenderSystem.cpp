#include <BLIB/Systems/RenderSystem.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace sys
{
RenderSystem::RenderSystem(rc::Renderer& r)
: renderer(r) {}

void RenderSystem::init(engine::Engine&) {}

void RenderSystem::update(std::mutex&, float) { renderer.renderFrame(); }

} // namespace sys
} // namespace bl
