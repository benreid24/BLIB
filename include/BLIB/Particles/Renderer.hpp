#ifndef BLIB_PARTICLES_RENDERER_HPP
#define BLIB_PARTICLES_RENDERER_HPP

namespace bl
{
namespace pcl
{
/**
 * @brief Base class for all renderer plugins for particle managers. Renderer plugins create ECS
 *        entities and interact with the renderer. Specialize this class or provide a custom class
 *        to the template argument to the particle manager
 *
 * @tparam T The type of particle to render
 * @ingroup Particles
 */
template<typename T>
class Renderer {
public:
    // TODO - renderer interface
};

} // namespace pcl
} // namespace bl

#endif
