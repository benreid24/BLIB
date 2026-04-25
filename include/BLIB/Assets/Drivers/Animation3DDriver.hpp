#ifndef BLIB_ASSETS_DRIVERS_ANIMATION3DDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_ANIMATION3DDRIVER_HPP

#include <BLIB/Assets/DriverGeneric.hpp>
#include <BLIB/Models/Animation.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Driver for 3d model animations
 *
 * @ingroup Assets
 */
using Animation3DDriver = as::DriverGeneric<mdl::Animation, util::TemplateString{"Animation3D"}>;

} // namespace asi
} // namespace bl

#endif
