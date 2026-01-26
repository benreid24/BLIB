#include <BLIB/Interfaces/GUI/RenderSettings.hpp>

namespace bl
{
namespace gui
{
namespace
{
constexpr float Padding = 0.f;
}

void RenderSettings::merge(const RenderSettings& settings) {
    if (settings.horizontalAlignment.has_value())
        horizontalAlignment = settings.horizontalAlignment;
    if (settings.verticalAlignment.has_value()) verticalAlignment = settings.verticalAlignment;
    if (settings.fillColor.has_value()) fillColor = settings.fillColor;
    if (settings.outlineColor.has_value()) outlineColor = settings.outlineColor;
    if (settings.outlineThickness.has_value()) outlineThickness = settings.outlineThickness;
    if (settings.secondaryFillColor.has_value()) secondaryFillColor = settings.secondaryFillColor;
    if (settings.secondaryOutlineColor.has_value())
        secondaryOutlineColor = settings.secondaryOutlineColor;
    if (settings.secondaryOutlineThickness.has_value())
        secondaryOutlineThickness = settings.secondaryOutlineThickness;
    if (settings.style.has_value()) style = settings.style;
    if (settings.characterSize.has_value()) characterSize = settings.characterSize;
}

void RenderSettings::promoteSecondaries() {
    fillColor        = secondaryFillColor;
    outlineColor     = secondaryOutlineColor;
    outlineThickness = secondaryOutlineThickness;
}

sf::Vector2f RenderSettings::calculatePosition(Alignment horizontalAlignment,
                                               Alignment verticalAlignment,
                                               const sf::FloatRect& region,
                                               const sf::Vector2f& size) {
    sf::Vector2f position;
    switch (horizontalAlignment) {
    case RenderSettings::Left:
        position.x = Padding;
        break;
    case RenderSettings::Right:
        position.x = region.size.x - size.x - Padding;
        break;
    case RenderSettings::Center:
    default:
        position.x = region.size.x * 0.5f - size.x * 0.5f;
        break;
    }

    switch (verticalAlignment) {
    case RenderSettings::Top:
        position.y = Padding;
        break;
    case RenderSettings::Bottom:
        position.y = region.size.y - size.y - Padding;
        break;
    case RenderSettings::Center:
    default:
        position.y = region.size.y * 0.5f - size.y * 0.5f;
        break;
    }

    return position;
}

} // namespace gui
} // namespace bl
