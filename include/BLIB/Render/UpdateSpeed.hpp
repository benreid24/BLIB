#ifndef BLIB_RENDER_UPDATESPEED_HPP
#define BLIB_RENDER_UPDATESPEED_HPP

#include <cstdint>

namespace bl
{
namespace gfx
{
/**
 * @brief Enum representing how frequently an object is expected to be updated
 *
 * @ingroup Renderer
 */
enum struct UpdateSpeed : std::uint8_t {
    /// Objects or data are not expected to update often after creation
    Static,
    /// Objects or data are expected to update once per frame or similar
    Dynamic
};

} // namespace gfx
} // namespace bl

#endif
