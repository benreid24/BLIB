#include <BLIB/Render/Graph/InitContext.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
InitContext::InitContext(engine::Engine& e, Renderer& r, vk::VulkanLayer& vs, RenderTarget& rt,
                         Scene* s)
: engine(e)
, renderer(r)
, vulkanState(vs)
, target(rt)
, scene(s)
, index(0) {}

InitContext::InitContext(const InitContext& ctx, unsigned int idx)
: engine(ctx.engine)
, renderer(ctx.renderer)
, vulkanState(ctx.vulkanState)
, target(ctx.target)
, scene(ctx.scene)
, index(idx) {}

sr::ShaderResourceStore& InitContext::getShaderResourceStore(sr::StoreKey key) const {
    switch (key) {
    case sr::StoreKey::Global:
        return renderer.getGlobalShaderResources();
    case sr::StoreKey::Observer:
        return target.getShaderResources();
    case sr::StoreKey::Scene:
    default:
        return scene->getShaderResources();
    }
}

} // namespace rg
} // namespace rc
} // namespace bl
