#ifndef BLIB_PARTICLES_RENDERCOMPONENTMAP_HPP
#define BLIB_PARTICLES_RENDERCOMPONENTMAP_HPP

#include <BLIB/Particles/EcsComponent.hpp>

namespace bl
{
namespace pcl
{
/**
 * @brief Compile-time config map used by the default Renderer<T> implementation. By default
 *        EcsComponent is used to render the particle system. Specialize this struct to specify a
 *        custom component to use instead. The constructor must have the same signature as
 *        the EcsComponent constructor
 *
 * @tparam T The particle type to map to the renderable component
 * @ingroup Particles
 */
template<typename T>
struct RenderComponentMap {
    /// The ECS component to use to render the particle systems for particle type T
    using TComponent = EcsComponent;
};

} // namespace pcl
} // namespace bl

#endif
