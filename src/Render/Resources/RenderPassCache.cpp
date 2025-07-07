#include <BLIB/Render/Resources/RenderPassCache.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Renderer.hpp>
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
        id, id, renderer.vulkanState(), std::forward<vk::RenderPassParameters>(sceneParams));
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
    standardColorAttachment.format         = vk::TextureFormat::DefaultColorFormat;
    standardColorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    standardColorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    standardColorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    standardColorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    standardColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    standardColorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    standardColorAttachment.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format         = renderer.vulkanState().findDepthFormat();
    depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
    depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription highPrecisionVecAttachment{};
    highPrecisionVecAttachment.format         = renderer.vulkanState().findHighPrecisionFormat();
    highPrecisionVecAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    highPrecisionVecAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    highPrecisionVecAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    highPrecisionVecAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    highPrecisionVecAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    highPrecisionVecAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    highPrecisionVecAttachment.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // block fragment shader reads until pass is done
    VkSubpassDependency postPassRenderCompleteDep{};
    postPassRenderCompleteDep.srcSubpass      = 0;
    postPassRenderCompleteDep.dstSubpass      = VK_SUBPASS_EXTERNAL;
    postPassRenderCompleteDep.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    postPassRenderCompleteDep.dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    postPassRenderCompleteDep.srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    postPassRenderCompleteDep.dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
    postPassRenderCompleteDep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    /// block fragment shader writes until prior pass is done
    VkSubpassDependency prePassRenderDoneDep{};
    prePassRenderDoneDep.srcSubpass      = VK_SUBPASS_EXTERNAL;
    prePassRenderDoneDep.dstSubpass      = 0;
    prePassRenderDoneDep.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    prePassRenderDoneDep.srcAccessMask   = 0;
    prePassRenderDoneDep.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    prePassRenderDoneDep.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    prePassRenderDoneDep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // block shadow map render until prior read done
    VkSubpassDependency shadowMapInputDependency{};
    shadowMapInputDependency.srcSubpass      = VK_SUBPASS_EXTERNAL;
    shadowMapInputDependency.dstSubpass      = 0;
    shadowMapInputDependency.srcStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    shadowMapInputDependency.dstStageMask    = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    shadowMapInputDependency.srcAccessMask   = VK_ACCESS_SHADER_READ_BIT;
    shadowMapInputDependency.dstAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    shadowMapInputDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // block shadow map read until generation is done
    VkSubpassDependency shadowMapOutputDependency{};
    shadowMapOutputDependency.srcSubpass      = 0;
    shadowMapOutputDependency.dstSubpass      = VK_SUBPASS_EXTERNAL;
    shadowMapOutputDependency.srcStageMask    = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    shadowMapOutputDependency.dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    shadowMapOutputDependency.srcAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    shadowMapOutputDependency.dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
    shadowMapOutputDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // do not write depth buffer until prior use is done
    VkSubpassDependency depthDependency{};
    depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depthDependency.dstSubpass = 0;
    depthDependency.srcStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDependency.dstStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDependency.srcAccessMask   = 0;
    depthDependency.dstAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    depthDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    vk::RenderPassParameters sceneParams;
    sceneParams.addAttachment(standardColorAttachment);
    sceneParams.addAttachment(depthAttachment);
    sceneParams.addSubpass(
        vk::RenderPassParameters::SubPass()
            .withAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withDepthAttachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            .build());
    sceneParams.addSubpassDependency(postPassRenderCompleteDep);
    sceneParams.addSubpassDependency(prePassRenderDoneDep);
    sceneParams.addSubpassDependency(depthDependency);
    createRenderPass(cfg::RenderPassIds::StandardAttachmentPass, sceneParams.build());

    // primary render pass for final swapchain compositing
    VkAttachmentDescription swapDefaultColorAttachment{};
    swapDefaultColorAttachment.format         = renderer.vulkanState().swapchain.swapImageFormat();
    swapDefaultColorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    swapDefaultColorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
    swapDefaultColorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    swapDefaultColorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    swapDefaultColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    swapDefaultColorAttachment.initialLayout  = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    swapDefaultColorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    vk::RenderPassParameters primaryParams;
    primaryParams.addAttachment(swapDefaultColorAttachment);
    primaryParams.addAttachment(depthAttachment);
    primaryParams.addSubpass(
        vk::RenderPassParameters::SubPass()
            .withAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withDepthAttachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            .build());
    primaryParams.addSubpassDependency(prePassRenderDoneDep);
    primaryParams.addSubpassDependency(depthDependency);
    createRenderPass(cfg::RenderPassIds::SwapchainPass, primaryParams.build());

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
    hdrParams.addSubpassDependency(postPassRenderCompleteDep);
    hdrParams.addSubpassDependency(prePassRenderDoneDep);
    hdrParams.addSubpassDependency(depthDependency);
    createRenderPass(cfg::RenderPassIds::HDRAttachmentPass, hdrParams.build());

    vk::RenderPassParameters bloomParams;
    bloomParams.addAttachment(hdrColorAttachment);
    bloomParams.addSubpass(vk::RenderPassParameters::SubPass()
                               .withAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                               .build());
    bloomParams.addSubpassDependency(postPassRenderCompleteDep);
    bloomParams.addSubpassDependency(prePassRenderDoneDep);
    createRenderPass(cfg::RenderPassIds::BloomPass, bloomParams.build());

    VkAttachmentDescription shadowMapAttachment = depthAttachment;
    shadowMapAttachment.format                  = renderer.vulkanState().findShadowMapFormat();
    shadowMapAttachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
    shadowMapAttachment.finalLayout             = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    shadowMapAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
    shadowMapAttachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_STORE;
    shadowMapAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    shadowMapAttachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
    shadowMapAttachment.finalLayout             = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    vk::RenderPassParameters shadowMapParams;
    shadowMapParams.addAttachment(shadowMapAttachment);
    shadowMapParams.addSubpass(
        vk::RenderPassParameters::SubPass()
            .withDepthAttachment(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            .build());
    shadowMapParams.addSubpassDependency(shadowMapInputDependency);
    shadowMapParams.addSubpassDependency(shadowMapOutputDependency);
    createRenderPass(cfg::RenderPassIds::ShadowMapPass, shadowMapParams.build());

    vk::RenderPassParameters deferredObjectParams;
    deferredObjectParams.addAttachment(standardColorAttachment);    // albedo
    deferredObjectParams.addAttachment(standardColorAttachment);    // specular + shininess
    deferredObjectParams.addAttachment(highPrecisionVecAttachment); // positions + lighting on/off
    deferredObjectParams.addAttachment(highPrecisionVecAttachment); // normals
    deferredObjectParams.addAttachment(depthAttachment);            // depth + stencil
    deferredObjectParams.addSubpass(
        vk::RenderPassParameters::SubPass()
            .withAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withAttachment(1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withAttachment(2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withAttachment(3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            .withDepthAttachment(4, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            .build());
    deferredObjectParams.addSubpassDependency(prePassRenderDoneDep);
    deferredObjectParams.addSubpassDependency(postPassRenderCompleteDep);
    deferredObjectParams.addSubpassDependency(shadowMapInputDependency);
    deferredObjectParams.addSubpassDependency(shadowMapOutputDependency);
    vk::RenderPassParameters deferredHDRObjectParams = deferredObjectParams;
    createRenderPass(cfg::RenderPassIds::DeferredObjectPass, deferredObjectParams.build());

    deferredHDRObjectParams.replaceAttachment(0, hdrColorAttachment);
    deferredHDRObjectParams.replaceAttachment(1, hdrColorAttachment);
    createRenderPass(cfg::RenderPassIds::HDRDeferredObjectPass, deferredHDRObjectParams.build());
}

} // namespace res
} // namespace rc
} // namespace bl
