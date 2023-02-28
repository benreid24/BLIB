#ifndef BLIB_ENGINE_EVENTS_WINDOWRESIZE_HPP
#define BLIB_ENGINE_EVENTS_WINDOWRESIZE_HPP

#include <SFML/Graphics/RenderWindow.hpp>

namespace bl
{
namespace engine
{
namespace event
{
/**
 * @brief Fired when the engine window is resized from a user event
 *
 * @ingroup EngineEvents
 *
 */
struct WindowResized {
    sf::WindowBase& window;

    /**
     * @brief Construct a new Window Resized object
     *
     * @param w The engine window
     */
    WindowResized(sf::WindowBase& w)
    : window(w) {}
};

} // namespace event
} // namespace engine
} // namespace bl

#endif
