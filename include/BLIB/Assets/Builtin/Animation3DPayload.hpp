#ifndef BLIB_ASSETS_BUILTIN_ANIMATION3DPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_ANIMATION3DPAYLOAD_HPP

#include <BLIB/Assets/PayloadGeneric.hpp>
#include <BLIB/Models/Animation.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Payload type for 3d animations
 *
 * @ingroup Assets
 */
using Animation3DPayload = as::PayloadGeneric<mdl::Animation>;

} // namespace asi
} // namespace bl

#endif
