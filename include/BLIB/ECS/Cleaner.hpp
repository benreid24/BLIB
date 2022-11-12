#ifndef BLIB_ECS_CLEANER_HPP
#define BLIB_ECS_CLEANER_HPP

#include <BLIB/ECS/Registry.hpp>

namespace bl
{
namespace ecs
{
/**
 * @brief Helper class to automatically remove entities using RAII based cleanup. Useful for
 *        functions that build entities by adding many components where component creation may fail
 *        at arbitrary points.
 *
 * @ingroup ECS
 *
 */
class Cleaner {
public:
    /**
     * @brief Construct a new Cleaner object
     *
     * @param registry The registry being used
     * @param entity The entity being created
     */
    Cleaner(Registry& registry, Entity entity);

    /**
     * @brief Destroys the entity unless disarmed
     *
     */
    ~Cleaner();

    /**
     * @brief Prevents destruction of the entity
     *
     */
    void disarm();

private:
    Registry& registry;
    const Entity entity;
    bool armed;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline Cleaner::Cleaner(Registry& registry, Entity entity)
: registry(registry)
, entity(entity)
, armed(true) {}

inline Cleaner::~Cleaner() {
    if (armed) registry.destroyEntity(entity);
}

inline void Cleaner::disarm() { armed = false; }

} // namespace ecs
} // namespace bl

#endif
