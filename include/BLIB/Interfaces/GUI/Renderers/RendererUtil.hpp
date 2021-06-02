#ifndef BLIB_GUI_RENDERERS_RENDERERUTIL_HPP
#define BLIB_GUI_RENDERERS_RENDERERUTIL_HPP

#include <BLIB/Interfaces/GUI/Renderers/Renderer.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Utility struct containing some common useful methods for rendering
 *
 */
struct RendererUtil {
    /**
     * @brief Calculates the position to render an element at given its alignments and size
     *
     * @param horizontalAlignment Horizontal alignment of the element being rendered
     * @param verticalAlignment Vertical alignment of the element being rendered
     * @param region Area the element is to be rendered in
     * @param size The size of the element being rendered
     * @return sf::Vector2f The position to render the element at to respect alignments
     */
    static sf::Vector2f calculatePosition(RenderSettings::Alignment horizontalAlignment,
                                          RenderSettings::Alignment verticalAlignment,
                                          const sf::IntRect& region, const sf::Vector2f& size);

    /**
     * @brief Generates an sf::Text object that takes into account all settings
     *
     * @param text The text to render
     * @param acquisition The area to render inside of
     * @param settings The settings to apply
     * @param defaults Default settings to apply if settings is missing a parameter
     */
    static sf::Text buildRenderText(const std::string& text, const sf::IntRect& acquisition,
                                    const RenderSettings& settings,
                                    const RenderSettings& defaults = {});

    /**
     * @brief Renders a simple rectangle with the given render settings.
     *        Custom elements may use this to implement rendering
     *
     * @param target The target to render to
     * @param states The render states to use
     * @param area The rectangle to render
     * @param settings The settings to use
     * @param defaults Default settings to apply if settings is missing a parameter
     * @param useSecondary True to use secondary fill and outline from settings, false for primary
     */
    static void renderRectangle(sf::RenderTarget& target, sf::RenderStates states,
                                const sf::IntRect& area, const RenderSettings& settings,
                                const RenderSettings& defaults = {}, bool useSecondary = false);
};

} // namespace gui
} // namespace bl

#endif