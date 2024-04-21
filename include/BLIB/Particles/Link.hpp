#ifndef BLIB_PARTICLES_LINK_HPP
#define BLIB_PARTICLES_LINK_HPP

#include <BLIB/Particles/RenderConfigMap.hpp>
#include <cstddef>

namespace bl
{
namespace pcl
{
/**
 * @brief ECS component that provides the link between descriptor sets and particle systems
 *
 * @tparam T The particle type
 * @ingroup Particles
 */
template<typename T>
struct Link {
    using TGlobalPayload = typename RenderConfigMap<T>::GlobalShaderPayload;

    T* base;
    std::size_t len;
    TGlobalPayload* globals;
};

} // namespace pcl
} // namespace bl

#endif
