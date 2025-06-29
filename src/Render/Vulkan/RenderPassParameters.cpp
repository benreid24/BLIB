#include <BLIB/Render/Vulkan/RenderPassParameters.hpp>

#include <stdexcept>
#include <utility>

namespace bl
{
namespace rc
{
namespace vk
{
RenderPassParameters::RenderPassParameters() {
    attachments.reserve(4);
    subpasses.reserve(4);
    dependencies.reserve(4);
}

RenderPassParameters& RenderPassParameters::addSubpass(SubPass&& subpass) {
    subpasses.emplace_back(std::forward<SubPass>(subpass));
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

RenderPassParameters&& RenderPassParameters::build() {
    if (subpasses.empty()) {
        throw std::runtime_error("RenderPass must have at least one subpass");
    }
    if (attachments.empty()) {
        throw std::runtime_error("RenderPass must have at least one attachment");
    }
    return std::move(*this);
}

RenderPassParameters::SubPass::SubPass() { colorAttachments.reserve(4); }

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

RenderPassParameters::SubPass&& RenderPassParameters::SubPass::build() { return std::move(*this); }

} // namespace vk
} // namespace rc
} // namespace bl
