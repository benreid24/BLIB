#ifndef BLIB_PARTICLES_LINK_HPP
#define BLIB_PARTICLES_LINK_HPP

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
    T* base;
    std::size_t len;
};

} // namespace pcl
} // namespace bl

#endif
