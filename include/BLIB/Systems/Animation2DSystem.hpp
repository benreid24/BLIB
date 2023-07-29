#ifndef BLIB_SYSTEMS_ANIMATION2DPLAYER_HPP
#define BLIB_SYSTEMS_ANIMATION2DPLAYER_HPP

#include <BLIB/Components/Animation2DPlayer.hpp>
#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/Engine/System.hpp>

namespace bl
{
namespace sys
{
/**
 * @brief Systems that updates and advances all 2d animations
 *
 * @ingroup Systems
 */
class Animation2DSystem : public engine::System {
public:
    /**
     * @brief Creates the animation system
     */
    Animation2DSystem();

    /**
     * @brief Destroys the animation system
     */
    virtual ~Animation2DSystem() = default;

private:
    ecs::ComponentPool<com::Animation2DPlayer>* players;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt) override;
};

} // namespace sys
} // namespace bl

#endif
