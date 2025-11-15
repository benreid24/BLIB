#include <BLIB/Render/ShaderResources/ShadowMapShaderResource.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
void ShadowMapShaderResource::createImages() {
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

void ShadowMapShaderResource::init(engine::Engine& engine, RenderTarget&) {
    renderer = &engine.renderer();
    createImages();
    subscribe(renderer->getSignalChannel());
    emitter.connect(renderer->getSignalChannel());
}

void ShadowMapShaderResource::cleanup() {
    // handled by dtor
}

void ShadowMapShaderResource::performTransfer() { dirtyFrames = dirtyFrames >> 1; }

void ShadowMapShaderResource::copyFromSource() {
    // noop
}

bool ShadowMapShaderResource::dynamicDescriptorUpdateRequired() const { return dirtyFrames != 0; }

bool ShadowMapShaderResource::staticDescriptorUpdateRequired() const { return dirtyFrames != 0; }

void ShadowMapShaderResource::process(const event::SettingsChanged& e) {
    if (e.setting == event::SettingsChanged::ShadowMapResolution) {
        createImages();
        emitter.emit<event::ShadowMapsInvalidated>({this});
    }
}

} // namespace sri
} // namespace rc
} // namespace bl
