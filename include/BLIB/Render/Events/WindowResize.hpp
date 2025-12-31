#ifndef BLIB_RENDERER_EVENTS_WINDOWRESIZE_HPP
#define BLIB_RENDERER_EVENTS_WINDOWRESIZE_HPP

#include <BLIB/Render/Window.hpp>

namespace bl
{
namespace rc
{
namespace event
{
/**
 * @brief Fired when the renderer window is resized from a user event
 *
 * @ingroup Renderer
 */
struct WindowResized {
    RenderWindow& window;

    /**
     * @brief Construct a new Window Resized object
     *
     * @param w The render window
     */
    WindowResized(RenderWindow& w)
    : window(w) {}
};

} // namespace event
} // namespace rc
} // namespace bl

#endif
