#ifndef BLIB_RENDER_SYSTEMS_RENDERSYSTEM_HPP
#define BLIB_RENDER_SYSTEMS_RENDERSYSTEM_HPP

#include <BLIB/Engine/System.hpp>

namespace bl
{
namespace rc
{
class Renderer;

namespace sys
{
/**
 * @brief System that triggers the renderer each frame
 *
 * @ingroup Renderer
 */
class RenderSystem : public engine::System {
public:
    /**
     * @brief Creates a new render system
     *
     * @param renderer The renderer instance
     */
    RenderSystem(Renderer& renderer);

    /**
     * @brief Destroys the render system
     */
    virtual ~RenderSystem() = default;

private:
    Renderer& renderer;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt) override;
};

} // namespace sys
} // namespace rc
} // namespace bl

#endif
