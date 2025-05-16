#include <BLIB/Render/Graph/Assets/ShadowMapAsset.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
ShadowMapAsset::ShadowMapAsset(std::string_view tag)
: Asset(tag) {}

void ShadowMapAsset::doCreate(engine::Engine& engine, Renderer& renderer, RenderTarget* observer) {
    VkRenderPass renderPass =
        renderer.renderPassCache().getRenderPass(Config::RenderPassIds::ShadowMapPass).rawPass();

    // TODO - events for renderer settings change
    auto commandBuffer = renderer.vulkanState().sharedCommandPool.createBuffer();
    for (auto& map : spotShadowMaps) {
        map.image.create(renderer.vulkanState(),
                         vk::Image::Type::Image2D,
                         renderer.vulkanState().findDepthFormat(),
                         VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                         renderer.getSettings().getShadowMapResolution(),
                         VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                         0,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         0,
                         VK_IMAGE_ASPECT_DEPTH_BIT);
        map.image.clearDepthAndPrepareForSampling(commandBuffer);
        map.attachmentSet.emplace(map.image);
        map.framebuffer.create(renderer.vulkanState(), renderPass, map.attachmentSet.value());
    }
    for (auto& map : pointShadowMaps) {
        map.image.create(renderer.vulkanState(),
                         vk::Image::Type::Cubemap,
                         renderer.vulkanState().findDepthFormat(),
                         VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                         renderer.getSettings().getShadowMapResolution(),
                         VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                         0,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         0,
                         VK_IMAGE_ASPECT_DEPTH_BIT);
        map.image.clearDepthAndPrepareForSampling(commandBuffer);
        map.attachmentSet.emplace(map.image);
        map.framebuffer.create(renderer.vulkanState(), renderPass, map.attachmentSet.value());
    }
    commandBuffer.submit();
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
