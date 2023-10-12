#ifndef BLIB_SYSTEMS_TOGGLERSYSTEM_HPP
#define BLIB_SYSTEMS_TOGGLERSYSTEM_HPP

#include <BLIB/Components/Toggler.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/Engine/System.hpp>

namespace bl
{
namespace sys
{
/**
 * @brief Basic system to update Toggler components
 *
 * @ingroup Systems
 */
class TogglerSystem : public engine::System {
public:
    /**
     * @brief Creates the system
     */
    TogglerSystem() = default;

    /**
     * @brief Destroys the system
     */
    virtual ~TogglerSystem() = default;

private:
    ecs::ComponentPool<com::Toggler>* pool;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt, float, float, float) override;
};

} // namespace sys
} // namespace bl

#endif
