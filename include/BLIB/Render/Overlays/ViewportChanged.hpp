#ifndef BLIB_RENDER_OVERLAYS_VIEWPORTCHANGED_HPP
#define BLIB_RENDER_OVERLAYS_VIEWPORTCHANGED_HPP

namespace bl
{
namespace render
{
class Overlay;

namespace ovy
{
/**
 * @brief Event struct that is fired when an object's viewport is changed
 *
 * @ingroup Renderer
 */
struct ViewportChanged {
    /**
     * @brief Creates a new event
     *
     * @param overlay The overlay that the object is in
     * @param sceneId The id of the object in the overlay
     */
    ViewportChanged(Overlay* overlay, std::uint32_t sceneId)
    : overlay(overlay)
    , sceneId(sceneId) {}

    const Overlay* overlay;
    const std::uint32_t sceneId;
};

} // namespace ovy
} // namespace render
} // namespace bl

#endif
