#include <BLIB/Render/Graph/Assets/FinalSwapframe.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
FinalSwapframe::FinalSwapframe(vk::PerSwapFrame<vk::Framebuffer>& framebuffers,
                               VkViewport& viewport, VkRect2D& scissor)
: Asset(rg::AssetTags::FinalFrameOutput)
, framebuffers(framebuffers)
, viewport(viewport)
, scissor(scissor) {}

void FinalSwapframe::doCreate(engine::Engine&, Renderer&) {
    // noop, managed by swapchain
}

void FinalSwapframe::doPrepareForInput(const rg::ExecutionContext&) {
    // noop, handled by renderpass
}

void FinalSwapframe::doPrepareForOutput(const rg::ExecutionContext&) {
    // noop, handled by renderpass
}

} // namespace rgi
} // namespace rc
} // namespace bl
