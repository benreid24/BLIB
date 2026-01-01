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
    subscribe(renderer.getSignalChannel());
    emitter.connect(renderer.getSignalChannel());
}

RenderPass::~RenderPass() {
    vkDestroyRenderPass(renderer.vulkanState().getDevice(), renderPass, nullptr);
}

void RenderPass::process(const event::SettingsChanged& e) {
    if (e.setting == event::SettingsChanged::Setting::AntiAliasing &&
        (createParams.getMSAABehavior() & RenderPassParameters::MSAABehavior::UseSettings)) {
        recreate();
    }
}

void RenderPass::process(const event::TextureFormatChanged& e) {
    for (std::uint32_t i = 0; i < createParams.attachments.size(); ++i) {
        if (createParams.semanticFormats[i] == e.semanticFormat) {
            recreate();
            break;
        }
    }
}

void RenderPass::recreate() {
    renderer.vulkanState().getCleanupManager().add(
        [device = renderer.vulkanState().getDevice(), rp = renderPass]() {
            vkDestroyRenderPass(device, rp, nullptr);
        });
    doCreate();
    emitter.emit<event::RenderPassInvalidated>({*this});
}

void RenderPass::doCreate() {
    const bool msaaEnabled =
        (createParams.msaaBehavior & RenderPassParameters::MSAABehavior::UseSettings) &&
        renderer.getSettings().getAntiAliasing() != Settings::AntiAliasing::None;
    const bool resolveAttachments =
        (createParams.msaaBehavior & RenderPassParameters::MSAABehavior::ResolveAttachments) &&
        msaaEnabled;

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
        if (msaaEnabled && resolveAttachments) {
            subpasses.back().pResolveAttachments = sp.resolveAttachments.data();
        }
    }

    // set sample count on color attachments based on MSAA behavior
    if (createParams.msaaBehavior & RenderPassParameters::MSAABehavior::UseSettings) {
        for (const auto& color : createParams.subpasses[0].colorAttachments) {
            auto& attachment   = createParams.attachments[color.attachment];
            attachment.samples = renderer.getSettings().getMSAASampleCount();
        }
    }

    // create local attachment list so we can reorder it
    auto semanticFormats = createParams.semanticFormats;
    auto attachments     = createParams.attachments;

    // attachments is always full length, need to remove resolve attachments if msaa is disabled
    if ((createParams.msaaBehavior & RenderPassParameters::MSAABehavior::ResolveAttachments) &&
        !msaaEnabled) {
        for (unsigned int i = 0; i < createParams.subpasses[0].resolveAttachments.size(); ++i) {
            attachments.erase(createParams.subpasses[0].colorAttachments.size());
            semanticFormats.erase(createParams.subpasses[0].colorAttachments.size());
        }
    }

    // update depth index if it exists
    if (createParams.subpasses[0].depthAttachment.has_value()) {
        auto& di = createParams.subpasses[0].depthAttachment.value().attachment;
        di       = msaaEnabled ? attachments.size() - 1 :
                                 createParams.subpasses[0].colorAttachments.size();
        attachments[di].samples =
            msaaEnabled ? renderer.getSettings().getMSAASampleCount() : VK_SAMPLE_COUNT_1_BIT;
    }

    // update formats based on semantic formats
    for (unsigned int i = 0; i < attachments.size(); ++i) {
        if (createParams.semanticFormats[i] != SemanticTextureFormat::NonSematic) {
            attachments[i].format =
                renderer.vulkanState().getTextureFormatManager().getFormat(semanticFormats[i]);
        }
    }

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments    = attachments.data();
    renderPassInfo.subpassCount    = subpasses.size();
    renderPassInfo.pSubpasses      = subpasses.data();
    renderPassInfo.dependencyCount = createParams.dependencies.size();
    renderPassInfo.pDependencies   = createParams.dependencies.data();

    if (vkCreateRenderPass(
            renderer.vulkanState().getDevice(), &renderPassInfo, nullptr, &renderPass) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass");
    }

    if (createParams.debugName) {
        renderer.vulkanState().setVulkanObjectDebugName(
            renderPass, VK_OBJECT_TYPE_RENDER_PASS, createParams.debugName);
    }
}

} // namespace vk
} // namespace rc
} // namespace bl
