#include <BLIB/Systems/RendererUpdateSystem.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace sys
{
RendererUpdateSystem::RendererUpdateSystem(rc::Renderer& r)
: renderer(r) {}

void RendererUpdateSystem::init(engine::Engine&) {}

void RendererUpdateSystem::update(std::mutex&, float dt, float realDt, float residual,
                                  float realResidual) {
    renderer.update(dt, realDt, residual, realResidual);
}

} // namespace sys
} // namespace bl
