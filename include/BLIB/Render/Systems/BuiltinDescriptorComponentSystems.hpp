#ifndef BLIB_RENDER_SYSTEMS_BUILTINDESCRIPTORCOMPONENTSYSTEMS_HPP
#define BLIB_RENDER_SYSTEMS_BUILTINDESCRIPTORCOMPONENTSYSTEMS_HPP

#include <BLIB/Components/Texture.hpp>
#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Systems/DescriptorComponentSystem.hpp>

namespace bl
{
namespace rc
{
namespace sys
{
/**
 * @brief Descriptor component system for 2d transforms
 * @ingroup Renderer
 */
using Transform2DDescriptorSystem = DescriptorComponentSystem<com::Transform2D, glm::mat4>;

/**
 * @brief Descriptor component system for 3d transforms
 * @ingroup Renderer
 */
using Transform3DDescriptorSystem = DescriptorComponentSystem<com::Transform3D, glm::mat4>;

/**
 * @brief Descriptor component system for textures
 * @ingroup Renderer
 */
using TextureDescriptorSystem = DescriptorComponentSystem<com::Texture, std::uint32_t>;

} // namespace sys
} // namespace rc
} // namespace bl

#endif
