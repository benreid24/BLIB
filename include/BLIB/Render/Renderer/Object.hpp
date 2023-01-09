#ifndef BLIB_RENDER_RENDERER_OBJECT_HPP
#define BLIB_RENDER_RENDERER_OBJECT_HPP

#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Render/Renderer/DrawParameters.hpp>
#include <BLIB/Render/Renderer/ObjectFlags.hpp>
#include <BLIB/Render/Uniforms/PushConstants.hpp>
#include <glad/vulkan.h>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
class Renderable;

/**
 * @brief Base renderable object. Everything that is rendered will reference an instance of this.
 *        Exists within the renderer and is batched per pipeline. Other classes reference into the
 *        renderer to modify this. The renderer itself will rip through these to render everything
 *
 * @ingroup Renderer
 */
struct Object {
    using Handle = container::ObjectPool<Object>::FixedRef;

    Renderable* owner;
    bool hidden;
    ObjectFlags flags;
    PushConstants frameData; // TODO - put in buffer and index into it?
    DrawParameters drawParams;
};

} // namespace render
} // namespace bl

#endif
