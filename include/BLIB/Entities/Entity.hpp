#ifndef BLIB_ENTITIES_ENTITY_HPP
#define BLIB_ENTITIES_ENTITY_HPP

#include <cstdint>
#include <mutex>

namespace bl
{
/// Contains the Entity Component System
namespace entity
{
/**
 * @brief Entity is simply an id. Components are added via a Registry and they own all the
 *        relevant properties. Systems can query lists of Entities with certain components and
 *        perform logic and updates
 *
 * @ingroup Entity
 *
 */
using Entity = std::uint64_t;

/**
 * @brief Special value to indicate that an Entity is not valid
 *
 * @ingroup Entities
 *
 */
constexpr Entity InvalidEntity = 0;

/**
 * @brief Singleton Entity generator. Can create sequential or random ids
 *
 * @ingroup Entity
 *
 */
class IdGenerator {
public:
    /**
     * @brief Creates a new Entity
     *
     */
    static Entity makeNew();

    /**
     * @brief Sets whether new Entities should be created with random or sequential ids.
     *        Sequential ids are guaranteed to be unique, but require a mutex and may slow down
     *        threads that have to wait for a lock. Random ids have a 1/2^64 chance of
     *        collision and are the default setting
     *
     * @param makeSequential True to generate ids sequentially, false for random
     */
    static void generateSequentialIds(bool makeSequential);

private:
    std::mutex idLock;
    Entity nextEntity;
    bool seq;

    IdGenerator();

    static IdGenerator& get();
};
} // namespace entity

} // namespace bl

#endif
