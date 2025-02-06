#ifndef BLIB_SYSTEMS_BUILTINDESCRIPTORCOMPONENTSYSTEMS_HPP
#define BLIB_SYSTEMS_BUILTINDESCRIPTORCOMPONENTSYSTEMS_HPP

#include <BLIB/Components/MaterialInstance.hpp>
#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Systems/DescriptorComponentSystem.hpp>

namespace bl
{
namespace sys
{
/**
 * @brief Descriptor component system for 3d transforms
 * @ingroup Systems
 */
using Transform3DDescriptorSystem = DescriptorComponentSystem<com::Transform3D, glm::mat4>;

/**
 * @brief Descriptor component system for textures
 * @ingroup Systems
 */
using TextureDescriptorSystem = DescriptorComponentSystem<com::MaterialInstance, std::uint32_t>;

/**
 * @brief Descriptor component system for materials
 * @ingroup Systems
 */
using MaterialDescriptorSystem =
    DescriptorComponentSystem<com::MaterialInstance, rc::mat::MaterialId>;

} // namespace sys
} // namespace bl

#endif
