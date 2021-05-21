#ifndef BLIB_GUI_RAWEVENT_HPP
#define BLIB_GUI_RAWEVENT_HPP

#include <SFML/Graphics/Transform.hpp>
#include <SFML/Window/Event.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Raw window event that can be propogated in response to window events
 *
 * @ingroup GUI
 *
 */
struct RawEvent {
    /**
     * @brief Convert the SFML event to a raw event
     *
     * @param event The SFML event to store
     * @param mousePos The global mouse position to use if none in the event
     * @param transform The transform to apply to the mouse position
     */
    RawEvent(const sf::Event& event, const sf::Vector2f& mousePos,
             const sf::Transform& transform);

    /**
     * @brief Create a new RawEvent with the local position transformed to the new
     *        parent's coordinate system
     *
     * @param parentRenderOffset The offset of the parent to transform to
     * @return RawEvent A new RawEvent with an updated local position
     */
    RawEvent transformToLocal(const sf::Vector2f& parentRenderOffset) const;

    /// The raw window event. Coordinates are in the global window system
    const sf::Event event;

    /// The position of the mouse in global window coordinates
    const sf::Vector2f globalMousePos;

    /// The transformed mouse position in parent local coordinates
    const sf::Vector2f localMousePos;

private:
    RawEvent(const sf::Event& event, const sf::Vector2f& gpos, const sf::Vector2f& lpos);
};

} // namespace gui
} // namespace bl

#endif