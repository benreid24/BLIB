#ifndef BLIB_RENDER_RENDERER_SCENEOBJECT_HPP
#define BLIB_RENDER_RENDERER_SCENEOBJECT_HPP

#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/Scenes/Key.hpp>

namespace bl
{
namespace rc
{
class Scene;

namespace rcom
{
struct SceneObjectRef;
}

/// Collection of core rendering functionality for rendering scenes
namespace scene
{

/**
 * @brief Base renderable object. Everything that is rendered will reference an instance of this.
 *        Exists within the renderer and is batched per pipeline. Other classes reference into the
 *        renderer to modify this. The renderer itself will rip through these to render everything
 *
 * @ingroup Renderer
 */
struct SceneObject {
    /**
     * @brief Construct a new SceneObject
     */
    SceneObject();

    /**
     * @brief Called by BatchedScene when when scene object addresses change
     */
    void updateRefToThis();

    bool hidden;
    Key sceneKey;
    prim::DrawParameters drawParams;

private:
    rcom::SceneObjectRef* refToThis;

    friend class bl::rc::Scene;
};

} // namespace scene
} // namespace rc
} // namespace bl

#endif
