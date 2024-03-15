#ifndef BLIB_PARTICLES_RENDERER_HPP
#define BLIB_PARTICLES_RENDERER_HPP

#include <BLIB/Logging.hpp>

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
    /**
     * @brief Destroys the renderer
     */
    virtual ~Renderer() = default;

    /**
     * @brief Called when the particle manager should be rendered in the given scene
     *
     * @param scene The scene to add to
     */
    void addToScene(rc::Scene* scene) {
        BL_LOG_CRITICAL << "Called addToScene on default renderer";
        (void)scene;
    }

    /**
     * @brief Called when the particles should be removed from its current scene
     */
    void removeFromScene() {
        BL_LOG_CRITICAL << "Called removeFromScene on default renderer";
        (void)scene;
    }
};

} // namespace pcl
} // namespace bl

#endif
