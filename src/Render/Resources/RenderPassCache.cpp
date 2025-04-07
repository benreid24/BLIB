#include <BLIB/Render/Resources/RenderPassCache.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>
#include <BLIB/Render/Vulkan/TextureFormat.hpp>

namespace bl
{
namespace rc
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
    if (!insertResult.second) {
        BL_LOG_WARN << "Duplicate creation of render pass with id: " << id;
    }
    return insertResult.first->second;
}

vk::RenderPass& RenderPassCache::getRenderPass(std::uint32_t id) {
    auto it = cache.find(id);
    if (it == cache.end()) {
        BL_LOG_CRITICAL << "Failed to find render pass with id: " << id;
        throw std::runtime_error("Failed to find gfx pass");
    }
    return it->second;
}

void RenderPassCache::addDefaults() {
    // scene render pass for observers
    VkAttachmentDescription standardColorAttachment{};
    standardColorAttachment.format         = vk::StandardAttachmentBuffers::DefaultColorFormat;
    standardColorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    standardColorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    standardColorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    standardColorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    standardColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    standardColorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    standardColorAttachment.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

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

    VkSubpassDependency swapchainAvailDep{};
    swapchainAvailDep.srcSubpass    = VK_SUBPASS_EXTERNAL;
    swapchainAvailDep.dstSubpass    = 0;
    swapchainAvailDep.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    swapchainAvailDep.srcAccessMask = 0;
    swapchainAvailDep.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    swapchainAvailDep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency depthDependency{};
    depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depthDependency.dstSubpass = 0;
    depthDependency.srcStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDependency.dstStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDependency.srcAccessMask = 0;
    depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    vk::RenderPassParameters sceneParams;
    sceneParams.addAttachment(standardColorAttachment);
    sceneParams.addAttachment(depthAttachment);
    sceneParams.addSubpass(
        vk::RenderPassParameters::SubPass()
            .withAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withDepthAttachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            .build());
    sceneParams.addSubpassDependency(renderCompleteDep);
    sceneParams.addSubpassDependency(swapchainAvailDep);
    sceneParams.addSubpassDependency(depthDependency);
    createRenderPass(Config::RenderPassIds::StandardAttachmentDefault, sceneParams.build());

    // primary render pass for final swapchain compositing
    VkAttachmentDescription swapDefaultColorAttachment{};
    swapDefaultColorAttachment.format         = renderer.vulkanState().swapchain.swapImageFormat();
    swapDefaultColorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    swapDefaultColorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    swapDefaultColorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    swapDefaultColorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    swapDefaultColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    swapDefaultColorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    swapDefaultColorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    vk::RenderPassParameters primaryParams;
    primaryParams.addAttachment(swapDefaultColorAttachment);
    primaryParams.addAttachment(depthAttachment);
    primaryParams.addSubpass(
        vk::RenderPassParameters::SubPass()
            .withAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withDepthAttachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            .build());
    primaryParams.addSubpassDependency(swapchainAvailDep);
    primaryParams.addSubpassDependency(depthDependency);
    createRenderPass(Config::RenderPassIds::SwapchainDefault, primaryParams.build());

    // HDR rendering
    VkAttachmentDescription hdrColorAttachment{};
    hdrColorAttachment.format         = vk::TextureFormat::HDRColor;
    hdrColorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    hdrColorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    hdrColorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    hdrColorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    hdrColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    hdrColorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    hdrColorAttachment.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    vk::RenderPassParameters hdrParams;
    hdrParams.addAttachment(hdrColorAttachment);
    hdrParams.addAttachment(depthAttachment);
    hdrParams.addSubpass(
        vk::RenderPassParameters::SubPass()
            .withAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withDepthAttachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            .build());
    hdrParams.addSubpassDependency(renderCompleteDep);
    hdrParams.addSubpassDependency(swapchainAvailDep);
    hdrParams.addSubpassDependency(depthDependency);
    createRenderPass(Config::RenderPassIds::HDRAttachmentDefault, hdrParams.build());
}

} // namespace res
} // namespace rc
} // namespace bl
