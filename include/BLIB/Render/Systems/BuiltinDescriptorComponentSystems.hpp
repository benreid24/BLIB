#ifndef BLIB_RENDER_SYSTEMS_BUILTINDESCRIPTORCOMPONENTSYSTEMS_HPP
#define BLIB_RENDER_SYSTEMS_BUILTINDESCRIPTORCOMPONENTSYSTEMS_HPP

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Systems/GenericDescriptorComponentSystem.hpp>
#include <BLIB/Transforms/3D/Transform3D.hpp>

namespace bl
{
namespace render
{
namespace sys
{
/**
 * @brief Descriptor component system for 3d transforms
 * @ingroup Renderer
 */
using Transform3DDescriptorSystem = GenericDescriptorComponentSystem<t3d::Transform3D, glm::mat4>;

} // namespace sys
} // namespace render
} // namespace bl

#endif
