#ifndef BLIB_ASSETS_BUILTIN_IMAGEPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_IMAGEPAYLOAD_HPP

#include <BLIB/Assets/PayloadGeneric.hpp>
#include <SFML/Graphics/Image.hpp>

namespace bl
{
namespace asi
{
/// Implementation details
namespace detail
{
/**
 * @brief Optional creation parameters for image assets
 *
 * @ingroup Assets
 */
struct ImagePayloadCreateParams : public as::CreateContext::CreateData {
    unsigned int width;
    unsigned int height;
    sf::Color fillColor;

    /// Takes priority over above parameters
    const sf::Image* sourceImage = nullptr;
};
} // namespace detail

/**
 * @brief Payload for sf::Image
 *
 * @ingroup Assets
 */
using ImagePayload = as::PayloadGeneric<sf::Image, detail::ImagePayloadCreateParams>;

} // namespace asi
} // namespace bl

#endif
