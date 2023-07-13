#ifndef BLIB_SYSTEMS_CAMERAUPDATESYSTEM_HPP
#define BLIB_SYSTEMS_CAMERAUPDATESYSTEM_HPP

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
 * @brief System that updates all observer cameras while the renderer is waiting on the prior
 *        frame to complete
 *
 * @ingroup Systems
 */
class CameraUpdateSystem : public engine::System {
public:
    /**
     * @brief Creates a new camera update system
     *
     * @param renderer The renderer instance
     */
    CameraUpdateSystem(rc::Renderer& renderer);

    /**
     * @brief Destroys the system
     */
    virtual ~CameraUpdateSystem() = default;

private:
    rc::Renderer& renderer;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt) override;
};

} // namespace sys
} // namespace bl

#endif
