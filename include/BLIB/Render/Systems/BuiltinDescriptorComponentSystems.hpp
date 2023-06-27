#ifndef BLIB_RENDER_SYSTEMS_BUILTINDESCRIPTORCOMPONENTSYSTEMS_HPP
#define BLIB_RENDER_SYSTEMS_BUILTINDESCRIPTORCOMPONENTSYSTEMS_HPP

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Components/Texture.hpp>
#include <BLIB/Render/Systems/DescriptorComponentSystem.hpp>
#include <BLIB/Transforms/2D/Transform2D.hpp>
#include <BLIB/Transforms/3D/Transform3D.hpp>

namespace bl
{
namespace gfx
{
namespace sys
{
/**
 * @brief Descriptor component system for 2d transforms
 * @ingroup Renderer
 */
using Transform2DDescriptorSystem = DescriptorComponentSystem<t2d::Transform2D, glm::mat4>;

/**
 * @brief Descriptor component system for 3d transforms
 * @ingroup Renderer
 */
using Transform3DDescriptorSystem = DescriptorComponentSystem<t3d::Transform3D, glm::mat4>;

/**
 * @brief Descriptor component system for textures
 * @ingroup Renderer
 */
using TextureDescriptorSystem = DescriptorComponentSystem<com::Texture, std::uint32_t>;

} // namespace sys
} // namespace gfx
} // namespace bl

#endif
