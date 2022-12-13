#ifndef BLIB_GUI_RENDERERS_RENDERSETTINGS_HPP
#define BLIB_GUI_RENDERERS_RENDERSETTINGS_HPP

#include <BLIB/Resources/Loader.hpp>
#include <SFML/Graphics.hpp>
#include <optional>

namespace bl
{
namespace gui
{
/**
 * @brief Data class to override rendering settings for GUI Elements. Overrides can be done by
 *        group or id. All fields are optional. Fields left empty will be defaulted by the
 *        renderer. Not all fields are valid for all Element types
 *
 * @ingroup GUI
 *
 */
struct RenderSettings {
    /**
     * @brief Controls where an Element is rendered with respect to its acquisition
     *
     */
    enum Alignment {
        Center, /// Element is centered on the axis
        Left,   /// Element is positioned on the left
        Right,  /// Element is positioned to the right
        Top,    /// Element is positioned on top
        Bottom  /// Element is positioned on the bottom
    };

    std::optional<Alignment> horizontalAlignment;
    std::optional<Alignment> verticalAlignment;

    std::optional<sf::Color> fillColor;
    std::optional<sf::Color> outlineColor;
    std::optional<float> outlineThickness;

    std::optional<sf::Color> secondaryFillColor;
    std::optional<sf::Color> secondaryOutlineColor;
    std::optional<float> secondaryOutlineThickness;

    std::optional<bl::resource::Ref<sf::Font>> font;
    std::optional<sf::Uint32> style;
    std::optional<unsigned int> characterSize;

    /**
     * @brief Merges a higher priority set of settings into this one, ignoring unset fields
     *
     * @param settings The settings to merge onto this object
     */
    void merge(const RenderSettings& settings);

    /**
     * @brief Copies the secondary colors and outline thickness to the primary spots
     *
     */
    void promoteSecondaries();
};

} // namespace gui
} // namespace bl

#endif
