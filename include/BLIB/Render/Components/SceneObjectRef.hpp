#ifndef BLIB_RENDER_COMPONENTS_SCENEOBJECTREF_HPP
#define BLIB_RENDER_COMPONENTS_SCENEOBJECTREF_HPP

#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <cstdint>

namespace bl
{
namespace rc
{
class Scene;

namespace scene
{
struct SceneObject;
}

/// Collection of ECS components for the renderer
namespace rcom
{
/**
 * @brief Basic struct representing a link between the ECS and a Scene's object buffer
 *
 * @ingroup Renderer
 */
struct SceneObjectRef {
    Scene* scene;
    scene::SceneObject* object;

    /**
     * @brief Creates an empty ref
     */
    SceneObjectRef()
    : scene(nullptr)
    , object(nullptr) {}
};

} // namespace rcom
} // namespace rc
} // namespace bl

#endif
