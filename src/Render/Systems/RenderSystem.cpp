#include <BLIB/Render/Systems/RenderSystem.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace sys
{
RenderSystem::RenderSystem(Renderer& r)
: renderer(r) {}

void RenderSystem::init(engine::Engine&) {}

void RenderSystem::update(std::mutex&, float) { renderer.renderFrame(); }

} // namespace sys
} // namespace rc
} // namespace bl
