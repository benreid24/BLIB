#ifndef BLIB_RENDER_EVENTS_SCENEDESTROYED_HPP
#define BLIB_RENDER_EVENTS_SCENEDESTROYED_HPP

namespace bl
{
namespace render
{
class Scene;

/// Collection of events related to the renderer
namespace event
{
/**
 * @brief Fired when a scene is destroyed. Systems should subscribe to this event in order to
 *        correctly update objects that were members of the destroyed scene
 *
 * @ingroup Renderer
 */
struct SceneDestroyed {
    Scene* scene;
};

} // namespace event
} // namespace render
} // namespace bl

#endif
