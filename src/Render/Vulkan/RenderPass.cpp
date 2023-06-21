#include <BLIB/Render/Vulkan/RenderPass.hpp>

namespace bl
{
namespace gfx
{
namespace vk
{
RenderPass::RenderPass(VulkanState& vs, RenderPassParameters&& params)
: vulkanState(vs) {
    std::vector<VkSubpassDescription> subpasses;
    subpasses.reserve(params.subpasses.size());
    for (const auto& sp : params.subpasses) {
        subpasses.emplace_back();
        subpasses.back().pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpasses.back().colorAttachmentCount = sp.colorAttachments.size();
        subpasses.back().pColorAttachments    = sp.colorAttachments.data();
        if (sp.depthAttachment.has_value()) {
            subpasses.back().pDepthStencilAttachment = &sp.depthAttachment.value();
        }
    }

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = params.attachments.size();
    renderPassInfo.pAttachments    = params.attachments.data();
    renderPassInfo.subpassCount    = subpasses.size();
    renderPassInfo.pSubpasses      = subpasses.data();
    renderPassInfo.dependencyCount = params.dependencies.size();
    renderPassInfo.pDependencies   = params.dependencies.data();

    if (vkCreateRenderPass(vulkanState.device, &renderPassInfo, nullptr, &renderPass) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create gfx pass");
    }
}

RenderPass::~RenderPass() { vkDestroyRenderPass(vulkanState.device, renderPass, nullptr); }

} // namespace vk
} // namespace gfx
} // namespace bl
