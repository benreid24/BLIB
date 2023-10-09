#ifndef BLIB_SYSTEMS_RENDERERUPDATESYSTEM_HPP
#define BLIB_SYSTEMS_RENDERERUPDATESYSTEM_HPP

#include <BLIB/Engine/System.hpp>

namespace bl
{
namespace rc
{
class Renderer;
}

namespace sys
{
/**
 * @brief System that updates all observer cameras and task graphs while the renderer is waiting on
 *        the prior frame to complete
 *
 * @ingroup Systems
 */
class RendererUpdateSystem : public engine::System {
public:
    /**
     * @brief Creates a new camera update system
     *
     * @param renderer The renderer instance
     */
    RendererUpdateSystem(rc::Renderer& renderer);

    /**
     * @brief Destroys the system
     */
    virtual ~RendererUpdateSystem() = default;

private:
    rc::Renderer& renderer;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt, float, float, float) override;
};

} // namespace sys
} // namespace bl

#endif
