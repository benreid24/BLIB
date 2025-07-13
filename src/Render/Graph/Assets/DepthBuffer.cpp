#include <BLIB/Render/Graph/Assets/DepthBuffer.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/RenderTarget.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
DepthBuffer::DepthBuffer()
: Asset(rg::AssetTags::DepthBuffer)
, engine(nullptr)
, mode(FullScreen)
, cleared(false) {}

void DepthBuffer::doCreate(engine::Engine& e, Renderer&, RenderTarget* target) {
    engine          = &e;
    const auto size = getSize(target->getRegionSize());
    createAttachment(size);
}

void DepthBuffer::onResize(glm::u32vec2 targetSize) {
    if (engine) {
        const auto newSize = getSize(targetSize);
        if (buffer.getSize().width != newSize.x || buffer.getSize().height != newSize.y) {
            buffer.resize(newSize, false);
        }
    }
}

void DepthBuffer::setSizeMode(SizeMode m) { mode = m; }

void DepthBuffer::clear(VkCommandBuffer commandBuffer) {
    buffer.clearDepthAndTransition(commandBuffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void DepthBuffer::onReset() { cleared = false; }

glm::u32vec2 DepthBuffer::getSize(const glm::u32vec2& targetSize) const {
    const auto windowSize = engine ? engine->window().getSfWindow().getSize() :
                                     sf::Vector2u(targetSize.x, targetSize.y);
    switch (mode) {
    case Target:
        return targetSize;

    case FullScreen:
    default:
        return {windowSize.x, windowSize.y};
    }
}

void DepthBuffer::doStartOutput(const rg::ExecutionContext& ctx) {
    if (!cleared) {
        cleared = true;
        clear(ctx.commandBuffer);
    }
}

void DepthBuffer::observe(const event::SettingsChanged& event) {
    if (event.setting == event::SettingsChanged::Setting::AntiAliasing) {
        createAttachment({buffer.getSize().width, buffer.getSize().height});
    }
}

void DepthBuffer::createAttachment(const glm::u32vec2& size) {
    auto& r = engine->renderer();

    buffer.create(r.vulkanState(),
                  vk::Image::Type::Image2D,
                  r.vulkanState().findDepthFormat(),
                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                  {size.x, size.y},
                  VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                  VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                  0,
                  VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM,
                  r.getSettings().getMSAASampleCount());
}

void DepthBuffer::ensureValid(const glm::u32vec2& size, VkSampleCountFlagBits samples) {
    if (engine) {
        if (buffer.getSize().width != size.x || buffer.getSize().height != size.y ||
            buffer.getSampleCount() != samples) {
            createAttachment(size);
        }
    }
}

} // namespace rgi
} // namespace rc
} // namespace bl
