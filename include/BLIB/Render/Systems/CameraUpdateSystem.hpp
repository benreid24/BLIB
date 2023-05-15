#ifndef BLIB_RENDER_SYSTEMS_CAMERAUPDATESYSTEM_HPP
#define BLIB_RENDER_SYSTEMS_CAMERAUPDATESYSTEM_HPP

#include <BLIB/Engine/System.hpp>

namespace bl
{
namespace render
{
class Renderer;

namespace sys
{
/**
 * @brief System that updates all observer cameras while the renderer is waiting on the prior
 *        frame to complete
 *
 * @ingroup Renderer
 */
class CameraUpdateSystem : public engine::System {
public:
    /**
     * @brief Creates a new camera update system
     *
     * @param renderer The renderer instance
     */
    CameraUpdateSystem(Renderer& renderer);

    /**
     * @brief Destroys the system
     */
    virtual ~CameraUpdateSystem() = default;

private:
    Renderer& renderer;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt) override;
};

} // namespace sys
} // namespace render
} // namespace bl

#endif
