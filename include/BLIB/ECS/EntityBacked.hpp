#ifndef BLIB_ECS_ENTITYBACKED_HPP
#define BLIB_ECS_ENTITYBACKED_HPP

#include <BLIB/ECS/Entity.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace ecs
{
/**
 * @brief Base class for ECS-backed objects. Useful for nice class interfaces while using ECS as
 *        backing for components. This class provides lifetime management of the entity itself
 *
 * @ingroup ECS
 */
class EntityBacked {
public:
    /**
     * @brief Does nothing
     */
    EntityBacked();

    /**
     * @brief Deleted
     */
    EntityBacked(const EntityBacked&) = delete;

    /**
     * @brief Assumes ownership of the entity, if any, from the given drawable
     *
     * @param move The drawable to take ownership from
     */
    EntityBacked(EntityBacked&& move);

    /**
     * @brief Destroys the ECS entity
     */
    ~EntityBacked();

    /**
     * @brief Deleted
     */
    EntityBacked& operator=(const EntityBacked&) = delete;

    /**
     * @brief Assumes ownership of the entity, if any, from the given drawable
     *
     * @param move The drawable to take ownership from
     * @return A reference to this object
     */
    EntityBacked& operator=(EntityBacked&& move);

    /**
     * @brief Returns the ECS entity of this drawable
     */
    ecs::Entity entity() const;

    /**
     * @brief Destroys the ECS entity and component
     */
    void destroy();

    /**
     * @brief Helper method to parent this drawable to another
     *
     * @param parent The parent of this drawable
     */
    void setParent(const EntityBacked& parent);

    /**
     * @brief Helper method to parent this drawable
     *
     * @param parent The parent entity of this drawable
     */
    void setParent(ecs::Entity parent);

    /**
     * @brief Helper method to remove the parent of this drawable, if any
     */
    void removeParent();

    /**
     * @brief Configures whether or not to destroy the ECS entity when this object is destroyed
     *
     * @param deleteEntity True to destroy, false to keep. Default is true
     */
    void deleteEntityOnDestroy(bool deleteEntity);

    /**
     * @brief Returns whether or not the underlying entity is destroyed when this object is
     */
    bool entityIsDeletedOnDestruction() const { return isDestroyed; }

    /**
     * @brief Returns whether or not the underlying entity exists
     * @return
     */
    bool exists() const;

protected:
    /**
     * @brief Creates the ECS entity for the drawable
     *
     * @param engine Game engine instance
     * @param flags The flags to create the entity with
     */
    void createEntityOnly(engine::Engine& engine, ecs::Flags flags = ecs::Flags::None);

    /**
     * @brief Initializes using an existing entity instead of creating one
     *
     * @param engine Game engine instance
     * @param entity The existing entity to use
     */
    void createFromExistingEntity(engine::Engine& engine, ecs::Entity entity);

    /**
     * @brief Returns the game engine instance. Only call after create()
     */
    engine::Engine& engine();

private:
    engine::Engine* enginePtr;
    ecs::Entity ecsId;
    bool isDestroyed;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline ecs::Entity EntityBacked::entity() const { return ecsId; }

inline engine::Engine& EntityBacked::engine() { return *enginePtr; }

} // namespace ecs
} // namespace bl

#endif
