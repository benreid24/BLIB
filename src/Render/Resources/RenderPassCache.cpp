#include <BLIB/Render/Resources/RenderPassCache.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>

namespace bl
{
namespace gfx
{
namespace res
{
RenderPassCache::RenderPassCache(Renderer& r)
: renderer(r) {}

void RenderPassCache::cleanup() { cache.clear(); }

vk::RenderPass& RenderPassCache::createRenderPass(std::uint32_t id,
                                                  vk::RenderPassParameters&& sceneParams) {
    const auto insertResult = cache.try_emplace(
        id, renderer.vulkanState(), std::forward<vk::RenderPassParameters>(sceneParams));
    if (!insertResult.second) { BL_LOG_WARN << "Duplicate creation of gfx pass with id: " << id; }
    return insertResult.first->second;
}

vk::RenderPass& RenderPassCache::getRenderPass(std::uint32_t id) {
    auto it = cache.find(id);
    if (it == cache.end()) {
        BL_LOG_CRITICAL << "Failed to find gfx pass with id: " << id;
        throw std::runtime_error("Failed to find gfx pass");
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

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = vk::StandardAttachmentBuffers::findDepthFormat(renderer.vulkanState());
    depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDependency renderCompleteDep{};
    renderCompleteDep.srcSubpass      = 0;
    renderCompleteDep.dstSubpass      = VK_SUBPASS_EXTERNAL;
    renderCompleteDep.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    renderCompleteDep.dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    renderCompleteDep.srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    renderCompleteDep.dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
    renderCompleteDep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    vk::RenderPassParameters sceneParams;
    sceneParams.addAttachment(sceneColorAttachment);
    sceneParams.addAttachment(depthAttachment);
    sceneParams.addSubpass(
        vk::RenderPassParameters::SubPass()
            .withAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withDepthAttachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            .build());
    sceneParams.addSubpassDependency(renderCompleteDep);
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

    vk::RenderPassParameters primaryParams;
    primaryParams.addAttachment(swapColorAttachment);
    primaryParams.addAttachment(depthAttachment);
    primaryParams.addSubpass(
        vk::RenderPassParameters::SubPass()
            .withAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withDepthAttachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            .build());
    primaryParams.addSubpassDependency(primaryRenderWaitImage);
    createRenderPass(Config::RenderPassIds::SwapchainPrimaryRender, primaryParams.build());
}

} // namespace res
} // namespace gfx
} // namespace bl
