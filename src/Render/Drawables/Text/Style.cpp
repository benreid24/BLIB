#include <BLIB/Render/Drawables/Text/Style.hpp>

#include <SFML/Graphics/Text.hpp>

namespace bl
{
namespace render
{
namespace draw
{
namespace txt
{
Style::Style()
: style(sf::Text::Style::Regular)
, fillColor{0.f, 0.f, 0.f, 1.f}
, outlineColor(fillColor)
, fontSize(18)
, outlineThickness(0) {}

} // namespace txt
} // namespace draw
} // namespace render
} // namespace bl
