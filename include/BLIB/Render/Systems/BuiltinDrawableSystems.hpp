#ifndef BLIB_RENDER_SYSTEMS_BUILTINDRAWABLESYSTEMS_HPP
#define BLIB_RENDER_SYSTEMS_BUILTINDRAWABLESYSTEMS_HPP

#include <BLIB/Render/Components/Mesh.hpp>
#include <BLIB/Render/Systems/GenericDrawableSystem.hpp>

namespace bl
{
namespace render
{
namespace sys
{
/**
 * @brief Engine system for managing entity meshes
 *
 * @ingroup Renderer
 */
using MeshSystem = GenericDrawableSystem<com::Mesh>;

} // namespace sys
} // namespace render
} // namespace bl

#endif
