#ifndef BLIB_COMPONENTS_MARKEDFORDEATH_HPP
#define BLIB_COMPONENTS_MARKEDFORDEATH_HPP

#include <BLIB/Engine/StateMask.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Component that can be added to an entity in order to destroy it after some time
 *
 * @ingroup Components
 */
struct MarkedForDeath {
    /**
     * @brief Marks the entity for immediate destruction on the next update
     */
    MarkedForDeath()
    : lifeLeft(0.f)
    , mask(engine::StateMask::All) {}

    /**
     * @brief Marks the entity for destruction after a number of seconds in the given engine state
     *
     * @param lifetime The simulation time, in seconds, to wait before destroying the entity
     * @param mask The mask for the engine states to consider when elapsing time
     */
    MarkedForDeath(float lifetime, engine::StateMask::V mask = engine::StateMask::All)
    : lifeLeft(lifetime)
    , mask(mask) {}

    float lifeLeft;
    engine::StateMask::V mask;
};

} // namespace com
} // namespace bl

#endif
