#ifndef BLIB_RENDER_PRIMITIVES_COLOR_HPP
#define BLIB_RENDER_PRIMITIVES_COLOR_HPP

#include <BLIB/Logging.hpp>
#include <SFML/Graphics/Color.hpp>
#include <glm/glm.hpp>
#include <initializer_list>

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

/**
 * @brief Helper method to disambiguate colors created with brace initializers
 *
 * @param color The color to convert
 * @return A normalized glm::vec4 representing the color
 */
inline glm::vec4 sfcol(std::initializer_list<float> color) {
    const float* c = color.begin();
    if (color.size() == 3) { return glm::vec4(c[0], c[1], c[2], 1.f); }
    else if (color.size() == 4) { return glm::vec4(c[0], c[1], c[2], c[3]); }
    else {
        BL_LOG_ERROR << "Unsupported number of color elements: " << color.size();
        return glm::vec4(1.f, 1.f, 1.f, 1.f);
    }
}

} // namespace bl

#endif
