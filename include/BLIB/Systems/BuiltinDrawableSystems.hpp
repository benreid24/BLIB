#ifndef BLIB_SYSTEMS_BUILTINDRAWABLESYSTEMS_HPP
#define BLIB_SYSTEMS_BUILTINDRAWABLESYSTEMS_HPP

#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Components/Sprite.hpp>
#include <BLIB/Systems/DrawableSystem.hpp>

namespace bl
{
namespace sys
{
/**
 * @brief Engine system for managing entity meshes
 *
 * @ingroup Systems
 */
using MeshSystem = DrawableSystem<com::Mesh>;

/**
 * @brief Engine system for managing entity sprites
 *
 * @ingroup Systems
 */
using SpriteSystem = DrawableSystem<com::Sprite>;

} // namespace sys
} // namespace bl

#endif
