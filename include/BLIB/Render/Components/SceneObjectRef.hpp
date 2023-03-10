#ifndef BLIB_RENDER_COMPONENTS_SCENEOBJECTREF_HPP
#define BLIB_RENDER_COMPONENTS_SCENEOBJECTREF_HPP

#include <BLIB/Render/Scenes/DrawParameters.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <cstdint>

namespace bl
{
namespace render
{
class Scene;
struct SceneObject;

/// Collection of ECS components for the renderer
namespace com
{
/**
 * @brief Basic struct representing a link between the ECS and a Scene's object buffer
 *
 * @ingroup Renderer
 */
struct SceneObjectRef {
    Scene* scene;
    SceneObject* object;
};

} // namespace com
} // namespace render
} // namespace bl

#endif
