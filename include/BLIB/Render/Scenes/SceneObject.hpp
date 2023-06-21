#ifndef BLIB_RENDER_RENDERER_SCENEOBJECT_HPP
#define BLIB_RENDER_RENDERER_SCENEOBJECT_HPP

#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/UpdateSpeed.hpp>
#include <glad/vulkan.h>
#include <glm/glm.hpp>

namespace bl
{
namespace gfx
{
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

    bool hidden;
    std::uint32_t sceneId;
    prim::DrawParameters drawParams;
};

} // namespace scene
} // namespace gfx
} // namespace bl

#endif
