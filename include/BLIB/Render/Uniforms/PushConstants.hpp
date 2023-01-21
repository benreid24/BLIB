#ifndef BLIB_RENDER_UNIFORMS_PUSHCONSTANTS_HPP
#define BLIB_RENDER_UNIFORMS_PUSHCONSTANTS_HPP

#include <cstdint>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
/**
 * @brief Universal push-constant struct that is sent with every Object that is rendered
 *
 * @ingroup Renderer
 */
struct PushConstants {
    glm::mat4 transform;
    //std::uint32_t index;
};

} // namespace render
} // namespace bl

#endif
