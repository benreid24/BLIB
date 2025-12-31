#include <BLIB/Render/ShaderResources/DepthBufferShaderResource.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
DepthBufferShaderResource::DepthBufferShaderResource()
: engine(nullptr)
, owner(nullptr)
, mode(FullScreen)
, dirtyFrames(0) {}

void DepthBufferShaderResource::clear(VkCommandBuffer commandBuffer) {
    buffer.clearAndTransition(commandBuffer,
                              VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                              {.depthStencil = {1.f, 0}});
}

void DepthBufferShaderResource::setSizeMode(SizeMode m) { mode = m; }

void DepthBufferShaderResource::init(engine::Engine& e, RenderTarget& o) {
    engine          = &e;
    owner           = &o;
    const auto size = getSize(o.getRegionSize());
    createAttachment(size);
    subscribe(e.renderer().getSignalChannel());
}

void DepthBufferShaderResource::cleanup() { buffer.deferDestroy(); }

void DepthBufferShaderResource::performTransfer() {
    // noop
}

void DepthBufferShaderResource::copyFromSource() {
    const auto requiredSize = getSize(owner->getRegionSize());
    if (buffer.getSize().width != requiredSize.x || buffer.getSize().height != requiredSize.y) {
        createAttachment(requiredSize);
    }
    dirtyFrames = dirtyFrames >> 1;
}

bool DepthBufferShaderResource::dynamicDescriptorUpdateRequired() const { return dirtyFrames != 0; }

bool DepthBufferShaderResource::staticDescriptorUpdateRequired() const { return dirtyFrames != 0; }

void DepthBufferShaderResource::process(const event::SettingsChanged& event) {
    if (event.setting == event::SettingsChanged::Setting::AntiAliasing) {
        if (buffer.getSampleCount() != event.settings.getMSAASampleCount()) {
            createAttachment({buffer.getSize().width, buffer.getSize().height});
        }
    }
}

void DepthBufferShaderResource::createAttachment(const glm::u32vec2& size) {
    auto& r = engine->renderer();

    buffer.create(r.vulkanState(),
                  {.type       = vk::ImageOptions::Type::Image2D,
                   .format     = r.vulkanState().findDepthFormat(),
                   .usage      = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                   .extent     = {size.x, size.y},
                   .aspect     = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                   .allocFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                   .samples    = r.getSettings().getMSAASampleCount()});
    dirtyFrames = 0x1 << cfg::Limits::MaxConcurrentFrames;
}

glm::u32vec2 DepthBufferShaderResource::getSize(const glm::u32vec2& targetSize) const {
    const auto windowSize = engine ? engine->renderer().getWindow().getSfWindow().getSize() :
                                     sf::Vector2u(targetSize.x, targetSize.y);
    switch (mode) {
    case Target:
        return targetSize;

    case FullScreen:
    default:
        return {windowSize.x, windowSize.y};
    }
}

void DepthBufferShaderResource::ensureValid(const glm::u32vec2& size,
                                            VkSampleCountFlagBits samples) {
    if (engine) {
        if (buffer.getSize().width < size.x || buffer.getSize().height < size.y ||
            buffer.getSampleCount() != samples) {
            createAttachment(size);
        }
    }
}

} // namespace sri
} // namespace rc
} // namespace bl
