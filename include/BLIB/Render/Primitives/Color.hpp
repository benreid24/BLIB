#ifndef BLIB_RENDER_PRIMITIVES_COLOR_HPP
#define BLIB_RENDER_PRIMITIVES_COLOR_HPP

#include <SFML/Graphics/Color.hpp>
#include <glm/glm.hpp>

namespace bl
{
/**
 * @brief Helper method to convert SFML colors to BLIB colors
 *
 * @param color The color to convert
 * @return A normalized glm::vec4 representing the color
 */
inline glm::vec4 sfcol(const sf::Color& color) {
    return {static_cast<float>(color.r) / 255.f,
            static_cast<float>(color.g) / 255.f,
            static_cast<float>(color.b) / 255.f,
            static_cast<float>(color.a) / 255.f};
}

} // namespace bl

#endif
