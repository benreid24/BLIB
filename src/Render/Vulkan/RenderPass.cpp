#include <BLIB/Render/Vulkan/RenderPass.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
RenderPass::RenderPass(std::uint32_t id, Renderer& r, RenderPassParameters&& params)
: id(id)
, renderer(r)
, createParams(std::move(params)) {
    doCreate();
    bl::event::Dispatcher::subscribe(this);
}

RenderPass::~RenderPass() {
    vkDestroyRenderPass(renderer.vulkanState().device, renderPass, nullptr);
}

void RenderPass::observe(const event::SettingsChanged& e) {
    if (e.setting == event::SettingsChanged::Setting::AntiAliasing &&
        createParams.getMSAABehavior() == RenderPassParameters::MSAABehavior::UseSettings) {
        renderer.vulkanState().cleanupManager.add(
            [device = renderer.vulkanState().device, rp = renderPass]() {
                vkDestroyRenderPass(device, rp, nullptr);
            });
        doCreate();
    }
}

void RenderPass::doCreate() {
    const bool msaaEnabled =
        createParams.msaaBehavior == RenderPassParameters::MSAABehavior::UseSettings &&
        renderer.getSettings().getAntiAliasing() != Settings::AntiAliasing::None;

    ctr::StaticVector<VkSubpassDescription, RenderPassParameters::MaxSubpassCount> subpasses;
    for (const auto& sp : createParams.subpasses) {
        subpasses.emplace_back(VkSubpassDescription{});
        subpasses.back().pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpasses.back().colorAttachmentCount    = sp.colorAttachments.size();
        subpasses.back().pColorAttachments       = sp.colorAttachments.data();
        subpasses.back().inputAttachmentCount    = sp.inputAttachments.size();
        subpasses.back().pInputAttachments       = sp.inputAttachments.data();
        subpasses.back().preserveAttachmentCount = sp.preserveAttachments.size();
        subpasses.back().pPreserveAttachments    = sp.preserveAttachments.data();
        if (sp.depthAttachment.has_value()) {
            subpasses.back().pDepthStencilAttachment = &sp.depthAttachment.value();
        }
        if (msaaEnabled && createParams.resolveAttachments) {
            subpasses.back().pResolveAttachments = sp.resolveAttachments.data();
        }
    }

    // set sample count on color attachments based on MSAA behavior
    if (createParams.msaaBehavior == RenderPassParameters::MSAABehavior::UseSettings) {
        for (const auto& color : createParams.subpasses[0].colorAttachments) {
            auto& attachment   = createParams.attachments[color.attachment];
            attachment.samples = renderer.getSettings().getMSAASampleCount();
        }
    }

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = createParams.attachments.size();
    renderPassInfo.pAttachments    = createParams.attachments.data();
    renderPassInfo.subpassCount    = subpasses.size();
    renderPassInfo.pSubpasses      = subpasses.data();
    renderPassInfo.dependencyCount = createParams.dependencies.size();
    renderPassInfo.pDependencies   = createParams.dependencies.data();

    // attachments is always full length, need to reduce if msaa is disabled
    if (createParams.resolveAttachments && !msaaEnabled) {
        renderPassInfo.attachmentCount -= createParams.subpasses[0].resolveAttachments.size();
    }

    if (vkCreateRenderPass(renderer.vulkanState().device, &renderPassInfo, nullptr, &renderPass) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass");
    }
}

} // namespace vk
} // namespace rc
} // namespace bl
