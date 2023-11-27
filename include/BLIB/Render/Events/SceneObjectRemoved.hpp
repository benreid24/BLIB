#ifndef BLIB_RENDER_EVENTS_SCENEOBJECTREMOVED_HPP
#define BLIB_RENDER_EVENTS_SCENEOBJECTREMOVED_HPP

#include <BLIB/ECS/Entity.hpp>

namespace bl
{
namespace rc
{
class Scene;

namespace event
{
/**
 * @brief Fired when an object is removed from a scene
 *
 * @ingroup Renderer
 */
struct SceneObjectRemoved {
    Scene* scene;
    ecs::Entity entity;
};

} // namespace event
} // namespace rc
} // namespace bl

#endif
