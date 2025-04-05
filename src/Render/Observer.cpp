#include <BLIB/Render/Observer.hpp>

#include <BLIB/Render/Graph/Assets/FinalSwapframeAsset.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
Observer::Observer(engine::Engine& e, Renderer& r, rg::AssetFactory& f, bool c, bool v)
: RenderTarget(e, r, f, false)
, isCommon(c)
, isVirtual(v) {
    renderingTo = graphAssets.putAsset<rgi::FinalSwapframeAsset>(
        renderer.getSwapframeBuffers(), viewport, scissor, clearColors, std::size(clearColors));
}

void Observer::assignRegion(const sf::Vector2u& windowSize,
                            const sf::Rect<std::uint32_t>& renderRegion, unsigned int count,
                            unsigned int i, bool topBottomFirst) {
    const std::uint32_t offsetX = (windowSize.x - renderRegion.width) / 2;
    const std::uint32_t offsetY = (windowSize.y - renderRegion.height) / 2;
    const sf::Vector2u uSize(renderRegion.getSize());
    const sf::Vector2f fSize(uSize);
    const sf::Vector2f fHalf(fSize * 0.5f);
    const sf::Vector2u uHalf(fHalf);
    const sf::Vector2i iHalf(uHalf);
    const VkExtent2D oldSize = scissor.extent;

    switch (count) {
    case 1:
        scissor.offset  = {0, 0};
        scissor.extent  = {uSize.x, uSize.y};
        viewport.x      = 0.f;
        viewport.y      = 0.f;
        viewport.width  = fSize.x;
        viewport.height = fSize.y;
        break;

    case 2:
        if (topBottomFirst) {
            scissor.extent  = {uSize.x, uHalf.y};
            viewport.width  = fSize.x;
            viewport.height = fHalf.y;
            viewport.x      = 0.f;
            if (i == 0) {
                scissor.offset = {0, 0};
                viewport.y     = 0.f;
            }
            else {
                scissor.offset = {0, iHalf.y};
                viewport.y     = fHalf.y;
            }
        }
        else {
            scissor.extent  = {uHalf.x, uSize.y};
            viewport.width  = fHalf.x;
            viewport.height = fSize.y;
            viewport.y      = 0.f;
            if (i == 0) {
                scissor.offset = {0, 0};
                viewport.x     = 0.f;
            }
            else {
                scissor.offset = {iHalf.x, 0};
                viewport.x     = fHalf.x;
            }
        }
        break;

    case 3:
    case 4:
        scissor.extent  = {uHalf.x, uHalf.y};
        viewport.width  = fHalf.x;
        viewport.height = fHalf.y;

        // top or bottom row
        if (i < 2) { // top row
            viewport.y       = 0.f;
            scissor.offset.y = 0;
        }
        else { // bottom row
            viewport.y       = fHalf.y;
            scissor.offset.y = uHalf.y;
        }

        // left or right column
        if (i % 2 == 0) { // left column
            viewport.x       = 0.f;
            scissor.offset.x = 0;
        }
        else { // right column
            viewport.x       = fHalf.x;
            scissor.offset.x = uHalf.x;
        }
        break;

    default:
        BL_LOG_ERROR << "Invalid observer count: " << count;
        break;
    }

    // add offsets for letterboxing
    viewport.x += offsetX;
    viewport.y += offsetY;
    scissor.offset.x += offsetX;
    scissor.offset.y += offsetY;

    if (scissor.extent.width != oldSize.width || scissor.extent.height != oldSize.height) {
        vkCheck(vkDeviceWaitIdle(renderer.vulkanState().device));
        graphAssets.notifyResize({scissor.extent.width, scissor.extent.height});
    }
}

void Observer::assignRegion(const VkRect2D& region) {
    if (!isVirtual) {
        throw std::runtime_error("Only virtual Observers may have manually assigned regions");
    }

    scissor           = region;
    viewport.x        = static_cast<float>(region.offset.x);
    viewport.y        = static_cast<float>(region.offset.y);
    viewport.width    = static_cast<float>(region.extent.width);
    viewport.height   = static_cast<float>(region.extent.height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    vkCheck(vkDeviceWaitIdle(renderer.vulkanState().device));
    graphAssets.notifyResize({scissor.extent.width, scissor.extent.height});
}

} // namespace rc
} // namespace bl
