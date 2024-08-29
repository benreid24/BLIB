#ifndef BLIB_SYSTEMS_MARKEDFORDEATH_HPP
#define BLIB_SYSTEMS_MARKEDFORDEATH_HPP

#include <BLIB/Components/MarkedForDeath.hpp>
#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Engine/System.hpp>

namespace bl
{
namespace sys
{
/**
 * @brief Engine system that updates and deletes entities with the MarkedForDeath component
 *
 * @ingroup Systems
 */
class MarkedForDeath : public engine::System {
public:
    /**
     * @brief Creates the system
     */
    MarkedForDeath();

    /**
     * @brief Destroys the system
     */
    virtual ~MarkedForDeath() = default;

private:
    engine::Engine* engine;
    ecs::Registry* registry;
    ecs::ComponentPool<com::MarkedForDeath>* pool;
    std::vector<ecs::Entity> toDelete;

    virtual void update(std::mutex&, float dt, float, float, float) override;
    virtual void init(engine::Engine& engine) override;
};

} // namespace sys
} // namespace bl

#endif
