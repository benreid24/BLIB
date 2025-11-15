#include <BLIB/Render/Graph/Assets/ShadowMapAsset.hpp>

#include <BLIB/Render/ShaderResources/ShaderResourceStore.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
ShadowMapAsset::ShadowMapAsset(std::string_view tag)
: Asset(tag, false) {}

void ShadowMapAsset::doCreate(const rg::InitContext& ctx) {
    maps = ctx.getShaderResourceStore(sr::StoreKey::Scene)
               .getShaderResourceWithKey(sri::ShadowMapResourceKey);
}

void ShadowMapAsset::doPrepareForInput(const rg::ExecutionContext&) {
    // handled by render pass
}

void ShadowMapAsset::doStartOutput(const rg::ExecutionContext&) {
    // handled by render pass
}

void ShadowMapAsset::doEndOutput(const rg::ExecutionContext&) {
    // handled by render pass
}

} // namespace rgi
} // namespace rc
} // namespace bl
