#ifndef BLIB_ASSETS_BUILTIN_IMAGEPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_IMAGEPAYLOAD_HPP

#include <BLIB/Assets/PayloadGeneric.hpp>
#include <SFML/Graphics/Image.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Payload for sf::Image
 *
 * @ingroup Assets
 */
using ImagePayload = as::PayloadGeneric<sf::Image>;

} // namespace asi
} // namespace bl

#endif
