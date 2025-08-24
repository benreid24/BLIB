#include <BLIB/Render/Graph/Assets/ShadowMapAsset.hpp>

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
ShadowMapAsset::ShadowMapAsset(std::string_view tag)
: Asset(tag)
, renderer(nullptr) {}

void ShadowMapAsset::doCreate(engine::Engine&, Renderer& r, RenderTarget*) {
    renderer = &r;
    createImages();
    subscribe(r.getSignalChannel());
    emitter.connect(r.getSignalChannel());
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

void ShadowMapAsset::process(const event::SettingsChanged& e) {
    if (e.setting == event::SettingsChanged::ShadowMapResolution) {
        createImages();
        emitter.emit<event::ShadowMapsInvalidated>({this});
    }
}

void ShadowMapAsset::createImages() {
    vk::RenderPass& renderPass =
        renderer->renderPassCache().getRenderPass(cfg::RenderPassIds::ShadowMapPass);

    const VkFormat depthFormat = renderer->vulkanState().findShadowMapFormat();
    const VkImageAspectFlags depthAspect =
        (depthFormat == VK_FORMAT_D32_SFLOAT || depthFormat == VK_FORMAT_D16_UNORM) ?
            VK_IMAGE_ASPECT_DEPTH_BIT :
            (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

    auto commandBuffer = renderer->vulkanState().sharedCommandPool.createBuffer();
    for (auto& map : spotShadowMaps) {
        map.image.create(
            renderer->vulkanState(),
            {.type       = vk::ImageOptions::Type::Image2D,
             .format     = depthFormat,
             .usage      = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
             .extent     = renderer->getSettings().getShadowMapResolution(),
             .aspect     = depthAspect,
             .viewAspect = VK_IMAGE_ASPECT_DEPTH_BIT});
        map.image.clearAndTransition(
            commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, {.depthStencil = {1.f, 0}});
        map.attachmentSet.emplace(map.image);
        map.framebuffer.create(renderer->vulkanState(), &renderPass, map.attachmentSet.value());
    }
    for (auto& map : pointShadowMaps) {
        map.image.create(
            renderer->vulkanState(),
            {.type       = vk::ImageOptions::Type::Cubemap,
             .format     = depthFormat,
             .usage      = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
             .extent     = renderer->getSettings().getShadowMapResolution(),
             .aspect     = depthAspect,
             .viewAspect = VK_IMAGE_ASPECT_DEPTH_BIT});
        map.image.clearAndTransition(
            commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, {.depthStencil = {1.f, 0}});
        map.attachmentSet.emplace(map.image);
        map.framebuffer.create(renderer->vulkanState(), &renderPass, map.attachmentSet.value());
    }
    commandBuffer.submit();
}

} // namespace rgi
} // namespace rc
} // namespace bl
