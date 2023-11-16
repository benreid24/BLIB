#ifndef BLIB_GUI_RENDERER_TEXTENTRYCOMPONENTBASE_HPP
#define BLIB_GUI_RENDERER_TEXTENTRYCOMPONENTBASE_HPP

#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
/**
 * @brief Intermediate base class for components of TextEntry elements
 *
 * @ingroup GUI
 */
class TextEntryComponentBase : public Component {
public:
    /**
     * @brief Destroys the component
     */
    virtual ~TextEntryComponentBase() = default;

    /**
     * @brief Called to determine what index the carat should move to when the text is clicked
     *
     * @param overlayPos The position in overlay coordinates
     * @return The closest text index of the given position
     */
    virtual unsigned int findCursorPosition(const sf::Vector2f& overlayPos) const = 0;

    /**
     * @brief Called when the carat changes position or toggles visibility
     */
    virtual void onCaratStateUpdate() = 0;

    /**
     * @brief Called when the carat flash timer should be reset
     */
    virtual void resetCaratFlash() = 0;

protected:
    /**
     * @brief Calls the base Component constructor
     *
     * @param highlightState The highlight state to pass to Component()
     */
    TextEntryComponentBase(Component::HighlightState highlightState)
    : Component(highlightState) {}
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
