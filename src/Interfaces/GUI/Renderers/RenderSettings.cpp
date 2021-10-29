#include <BLIB/Interfaces/GUI/Renderers/RenderSettings.hpp>

namespace bl
{
namespace gui
{
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

} // namespace gui
} // namespace bl
