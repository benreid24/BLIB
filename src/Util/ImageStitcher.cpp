#include <BLIB/Util/ImageStitcher.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <limits>

namespace bl
{
namespace util
{
ImageStitcher::ImageStitcher(const rc::Renderer& renderer, unsigned int padding)
: ImageStitcher(renderer.vulkanState().getPhysicalDeviceProperties().limits.maxImageDimension2D,
                padding) {}

ImageStitcher::ImageStitcher(unsigned int mw, unsigned int padding)
: maxWidth(mw)
, padding(padding) {}

const sf::Image& ImageStitcher::getStitchedImage() const { return image; }

glm::u32vec2 ImageStitcher::addImage(const sf::Image& img) {
    const unsigned int maxRowHeight = static_cast<float>(img.getSize().y) * 2.f;
    unsigned int bestRowDiff        = std::numeric_limits<unsigned int>::max();
    Row* bestRow                    = nullptr;

    for (Row& row : rows) {
        if (row.height >= img.getSize().y && row.height <= maxRowHeight) {
            if (row.width + img.getSize().x <= maxWidth) {
                const unsigned int diff = row.height - img.getSize().y;
                if (!bestRow || diff < bestRowDiff) {
                    bestRow     = &row;
                    bestRowDiff = diff;
                }
            }
        }
    }

    if (!bestRow) {
        if (image.getSize().y + img.getSize().y + padding > maxWidth) {
            BL_LOG_ERROR << "New stitched image row will exceed maximum height";
        }
        bestRow = &rows.emplace_back(image.getSize().y + padding, img.getSize().y);
    }

    const sf::Vector2u requiredSize(
        std::max(image.getSize().x, bestRow->width + img.getSize().x + padding),
        std::max(image.getSize().y, bestRow->y + bestRow->height));
    if (requiredSize.x > image.getSize().x || requiredSize.y > image.getSize().y) {
        sf::Image newImage;
        newImage.resize({requiredSize.x, requiredSize.y}, sf::Color::Transparent);
        newImage.copy(image, {0, 0});
        image = std::move(newImage);
    }

    const glm::u32vec2 pos(bestRow->width + padding, bestRow->y);
    bestRow->width += img.getSize().x + padding;
    image.copy(img, {pos.x, pos.y});
    return pos;
}

} // namespace util
} // namespace bl
