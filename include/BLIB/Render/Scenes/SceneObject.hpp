#ifndef BLIB_RENDER_RENDERER_SCENEOBJECT_HPP
#define BLIB_RENDER_RENDERER_SCENEOBJECT_HPP

#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Render/Scenes/DrawParameters.hpp>
#include <glad/vulkan.h>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
/**
 * @brief Base renderable object. Everything that is rendered will reference an instance of this.
 *        Exists within the renderer and is batched per pipeline. Other classes reference into the
 *        renderer to modify this. The renderer itself will rip through these to render everything
 *
 * @ingroup Renderer
 */
struct SceneObject {
    /// Enum representing how frequently an object is expected to be updated
    enum struct UpdateSpeed : std::uint8_t { Static, Dynamic };

    /**
     * @brief Construct a new SceneObject
     */
    SceneObject();

    bool hidden;
    std::uint32_t sceneId;
    DrawParameters drawParams;
};

} // namespace render
} // namespace bl

#endif
