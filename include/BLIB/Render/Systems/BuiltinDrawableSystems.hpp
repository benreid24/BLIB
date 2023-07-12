#ifndef BLIB_RENDER_SYSTEMS_BUILTINDRAWABLESYSTEMS_HPP
#define BLIB_RENDER_SYSTEMS_BUILTINDRAWABLESYSTEMS_HPP

#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Components/Sprite.hpp>
#include <BLIB/Render/Systems/DrawableSystem.hpp>

namespace bl
{
namespace rc
{
namespace sys
{
/**
 * @brief Engine system for managing entity meshes
 *
 * @ingroup Renderer
 */
using MeshSystem = DrawableSystem<com::Mesh>;

/**
 * @brief Engine system for managing entity sprites
 *
 * @ingroup Renderer
 */
using SpriteSystem = DrawableSystem<com::Sprite>;

} // namespace sys
} // namespace rc
} // namespace bl

#endif
