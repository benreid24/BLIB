#include <BLIB/Render/Resources/RenderPassCache.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/StandardImageBuffer.hpp>

namespace bl
{
namespace render
{
RenderPassCache::RenderPassCache(Renderer& r)
: renderer(r) {}

void RenderPassCache::cleanup() { cache.clear(); }

RenderPass& RenderPassCache::createRenderPass(std::uint32_t id,
                                              RenderPassParameters&& sceneParams) {
    const auto insertResult = cache.try_emplace(
        id, renderer.vulkanState(), std::forward<RenderPassParameters>(sceneParams));
    if (!insertResult.second) {
        BL_LOG_WARN << "Duplicate creation of render pass with id: " << id;
    }
    return insertResult.first->second;
}

RenderPass& RenderPassCache::getRenderPass(std::uint32_t id) {
    auto it = cache.find(id);
    if (it == cache.end()) {
        BL_LOG_CRITICAL << "Failed to find render pass with id: " << id;
        throw std::runtime_error("Failed to find render pass");
    }
    return it->second;
}

void RenderPassCache::addDefaults() {
    // scene render pass for observers
    VkAttachmentDescription sceneColorAttachment{};
    sceneColorAttachment.format         = VK_FORMAT_R8G8B8A8_SRGB;
    sceneColorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    sceneColorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    sceneColorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    sceneColorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    sceneColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    sceneColorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    sceneColorAttachment.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription sceneDepthAttachment{};
    sceneDepthAttachment.format  = StandardImageBuffer::findDepthFormat(renderer.vulkanState());
    sceneDepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    sceneDepthAttachment.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    sceneDepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    sceneDepthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    sceneDepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    sceneDepthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    sceneDepthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDependency sceneDepthDependency{}; // dont clear depth too soon
    sceneDepthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    sceneDepthDependency.dstSubpass = 0;
    sceneDepthDependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    sceneDepthDependency.srcAccessMask = 0;
    sceneDepthDependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    sceneDepthDependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    RenderPassParameters sceneParams;
    sceneParams.addAttachment(sceneColorAttachment);
    sceneParams.addAttachment(sceneDepthAttachment);
    sceneParams.addSubpass(
        RenderPassParameters::SubPass()
            .withAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withDepthAttachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            .build());
    sceneParams.addSubpassDependency(sceneDepthDependency);
    createRenderPass(Config::RenderPassIds::OffScreenSceneRender, sceneParams.build());

    // primary render pass for final swapchain compositing
    VkAttachmentDescription swapColorAttachment{};
    swapColorAttachment.format         = renderer.vulkanState().swapchain.swapImageFormat();
    swapColorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    swapColorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    swapColorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    swapColorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    swapColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    swapColorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    swapColorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkSubpassDependency primaryRenderWaitImage{}; // wait for image to be available
    primaryRenderWaitImage.srcSubpass    = VK_SUBPASS_EXTERNAL;
    primaryRenderWaitImage.dstSubpass    = 0;
    primaryRenderWaitImage.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    primaryRenderWaitImage.srcAccessMask = 0;
    primaryRenderWaitImage.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    primaryRenderWaitImage.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    RenderPassParameters primaryParams;
    primaryParams.addAttachment(swapColorAttachment);
    primaryParams.addSubpass(RenderPassParameters::SubPass()
                                 .withAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                                 .build());
    primaryParams.addSubpassDependency(primaryRenderWaitImage);
    createRenderPass(Config::RenderPassIds::SwapchainPrimaryRender, primaryParams.build());
}

} // namespace render
} // namespace bl
