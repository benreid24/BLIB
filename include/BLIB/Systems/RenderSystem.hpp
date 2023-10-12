#ifndef BLIB_SYSTEMS_RENDERSYSTEM_HPP
#define BLIB_SYSTEMS_RENDERSYSTEM_HPP

#include <BLIB/Engine/System.hpp>

namespace bl
{
namespace rc
{
class Renderer;
}

/// Collection of built-in engine systems
namespace sys
{
/**
 * @brief System that triggers the renderer each frame
 *
 * @ingroup Systems
 */
class RenderSystem : public engine::System {
public:
    /**
     * @brief Creates a new render system
     *
     * @param renderer The renderer instance
     */
    RenderSystem(rc::Renderer& renderer);

    /**
     * @brief Destroys the render system
     */
    virtual ~RenderSystem() = default;

private:
    rc::Renderer& renderer;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt, float, float, float) override;
};

} // namespace sys
} // namespace bl

#endif
