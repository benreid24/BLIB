#ifndef BLIB_RENDER_RENDERSYSTEM_HPP
#define BLIB_RENDER_RENDERSYSTEM_HPP

#include <BLIB/Render/Cameras.hpp>
#include <BLIB/Util/NonCopyable.hpp>

namespace bl
{
namespace render
{
/**
 * @brief Top level rendering system of the game engine
 *
 * @ingroup RenderSystem
 *
 */
class RenderSystem : private util::NonCopyable {
public:
    /**
     * @brief Construct a new Render System
     *
     */
    RenderSystem();

    /**
     * @brief The cameras in use by the render system
     *
     */
    Cameras& cameras();

    /**
     * @brief The cameras in use by the render system
     *
     */
    const Cameras& cameras() const;

    /**
     * @brief Updates the cameras
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    // TODO - implement proper rendering system

private:
    Cameras cameraSystem;
};

} // namespace render
} // namespace bl

#endif
