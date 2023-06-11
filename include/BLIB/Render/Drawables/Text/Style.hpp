#ifndef BLIB_RENDER_DRAWABLES_TEXT_STYLE_HPP
#define BLIB_RENDER_DRAWABLES_TEXT_STYLE_HPP

#include <SFML/Graphics/Font.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
namespace draw
{
namespace txt
{
struct Style {
    Style();

    const sf::Font* font;
    std::uint32_t style;
    glm::vec4 fillColor;
    glm::vec4 outlineColor;
    unsigned int fontSize;
    unsigned int outlineThickness;
};

} // namespace txt
} // namespace draw
} // namespace render
} // namespace bl

#endif
