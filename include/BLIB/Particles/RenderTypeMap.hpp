#ifndef BLIB_PARTICLES_RENDERTYPEMAP_HPP
#define BLIB_PARTICLES_RENDERTYPEMAP_HPP

#include <BLIB/Particles/Renderer.hpp>

namespace bl
{
namespace pcl
{

/**
 * @brief Compile-time type map to configure the Renderer type to use for a given
 *        particle type. Specialize this to set TRenderer to a custom type
 *
 * @tparam T The particle type being mapped
 * @ingroup Particles
 */
template<typename T>
struct RenderTypeMap {
    /// The renderer class to use to renderer the particle system
    using TRenderer = Renderer<T>;
};

} // namespace pcl
} // namespace bl

#endif
