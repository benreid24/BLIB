#include <BLIB/Render/Vulkan/RenderPassParameters.hpp>

#include <stdexcept>
#include <string>
#include <utility>

namespace bl
{
namespace rc
{
namespace vk
{
RenderPassParameters::RenderPassParameters()
: msaaBehavior(MSAABehavior::Disabled) {}

RenderPassParameters& RenderPassParameters::addSubpass(SubPass&& subpass) {
    subpasses.emplace_back(std::forward<SubPass>(subpass));
    return *this;
}

RenderPassParameters& RenderPassParameters::useSubpassOutputsAsInputs(std::uint32_t index,
                                                                      bool depth) {
    if (index == 0 || index >= subpasses.size()) {
        throw std::runtime_error("Invalid subpass index");
    }

    SubPass& prevPass  = subpasses[index - 1];
    const auto& inputs = prevPass.colorAttachments;
    if (inputs.empty()) {
        throw std::runtime_error("Subpass " + std::to_string(index - 1) +
                                 " has no color attachments to use as inputs");
    }

    const bool useDepth = depth && prevPass.depthAttachment.has_value();
    SubPass& subpass    = subpasses[index];
    subpass.inputAttachments.resize(inputs.size() + (useDepth ? 1 : 0));
    for (std::size_t i = 0; i < inputs.size(); ++i) {
        subpass.inputAttachments[i].attachment = inputs[i].attachment;
        subpass.inputAttachments[i].layout     = inputs[i].layout;
    }
    if (useDepth) {
        subpass.inputAttachments.back().attachment = prevPass.depthAttachment.value().attachment;
        subpass.inputAttachments.back().layout     = prevPass.depthAttachment.value().layout;
    }

    return *this;
}

RenderPassParameters& RenderPassParameters::addSubpassDependency(VkSubpassDependency dep) {
    dependencies.emplace_back(dep);
    return *this;
}

RenderPassParameters& RenderPassParameters::addAttachment(VkAttachmentDescription att) {
    attachments.emplace_back(att);
    return *this;
}

RenderPassParameters& RenderPassParameters::replaceAttachment(std::uint32_t i,
                                                              VkAttachmentDescription att) {
    if (i >= attachments.size()) { throw std::out_of_range("Attachment index out of range"); }
    attachments[i] = att;
    return *this;
}

RenderPassParameters& RenderPassParameters::withMSAABehavior(MSAABehavior behavior) {
    msaaBehavior = behavior;
    return *this;
}

RenderPassParameters&& RenderPassParameters::build() {
    if (subpasses.empty()) {
        throw std::runtime_error("RenderPass must have at least one subpass");
    }
    if (attachments.empty()) {
        throw std::runtime_error("RenderPass must have at least one attachment");
    }

    // regardless of msaa state we can populate the resolve attachments and append them to the
    // attachment list
    if (msaaBehavior & MSAABehavior::ResolveAttachments) {
        if (subpasses.size() > 1) {
            throw std::runtime_error("Resolve attachments can only be used with a single subpass");
        }
        auto& pass              = subpasses[0];
        pass.resolveAttachments = pass.colorAttachments;
        for (auto& resolve : pass.resolveAttachments) {
            resolve.attachment += pass.colorAttachments.size();
        }
        for (const auto& color : pass.colorAttachments) {
            const auto& src        = attachments[color.attachment];
            auto& resolved         = attachments.emplace_back(src);
            resolved.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            resolved.loadOp        = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            resolved.samples       = VK_SAMPLE_COUNT_1_BIT;
        }

        // move the depth attachment to the end of the list if it exists
        if (pass.depthAttachment.has_value()) {
            const auto depth = attachments[pass.depthAttachment.value().attachment];
            attachments.erase(pass.depthAttachment.value().attachment);
            attachments.emplace_back(depth);
            pass.depthAttachment.value().attachment = attachments.size() - 1;
        }
    }

    return std::move(*this);
}

RenderPassParameters::SubPass::SubPass() {}

RenderPassParameters::SubPass& RenderPassParameters::SubPass::withInputAttachment(
    std::uint32_t index, VkImageLayout layout) {
    inputAttachments.emplace_back();
    inputAttachments.back().attachment = index;
    inputAttachments.back().layout     = layout;
    return *this;
}

RenderPassParameters::SubPass& RenderPassParameters::SubPass::withAttachment(std::uint32_t i,
                                                                             VkImageLayout layout) {
    colorAttachments.emplace_back();
    colorAttachments.back().attachment = i;
    colorAttachments.back().layout     = layout;
    return *this;
}

RenderPassParameters::SubPass& RenderPassParameters::SubPass::withDepthAttachment(
    std::uint32_t i, VkImageLayout layout) {
    depthAttachment.emplace();
    depthAttachment.value().layout     = layout;
    depthAttachment.value().attachment = i;
    return *this;
}

RenderPassParameters::SubPass& RenderPassParameters::SubPass::withPreserveAttachment(
    std::uint32_t index) {
    preserveAttachments.emplace_back(index);
    return *this;
}

RenderPassParameters::SubPass&& RenderPassParameters::SubPass::build() { return std::move(*this); }

} // namespace vk
} // namespace rc
} // namespace bl
