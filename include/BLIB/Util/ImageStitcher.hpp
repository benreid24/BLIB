#ifndef BLIB_UTIL_IMAGESTITCHER_HPP
#define BLIB_UTIL_IMAGESTITCHER_HPP

#include <SFML/Graphics/Image.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace bl
{
namespace rc
{
class Renderer;
}

namespace util
{
/**
 * @brief Helper class to combine multiple images into one large atlas image
 *
 * @ingroup Util
 */
class ImageStitcher {
public:
    /**
     * @brief Creates the stitcher with the given max width and padding
     *
     * @param maxWidth The maximum width to grow to
     * @param padding The space between images to other images and the edges of the atlas
     */
    ImageStitcher(unsigned int maxWidth, unsigned int padding = 2);

    /**
     * @brief Creates the stitcher using the max texture size for the current hardware
     *
     * @param renderer The engine renderer instance
     * @param padding The space between images to other images and the edges of the atlas
     */
    ImageStitcher(const rc::Renderer& renderer, unsigned int padding = 2);

    /**
     * @brief Adds an image to the atlas and returns it's position in the atlas
     *
     * @param img The image to add
     * @return The position of the added image in the atlas
     */
    glm::u32vec2 addImage(const sf::Image& img);

    /**
     * @brief Returns the combined image
     */
    const sf::Image& getStitchedImage() const;

private:
    struct Row {
        unsigned int y;
        unsigned int height;
        unsigned int width;

        Row(unsigned int y, unsigned int height)
        : y(y)
        , height(height)
        , width(0) {}
    };

    const unsigned int maxWidth;
    const unsigned int padding;
    sf::Image image;
    std::vector<Row> rows;
};

} // namespace util
} // namespace bl

#endif
