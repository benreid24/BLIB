#ifndef BLIB_GRAPHICS_COMPONENTS_ENTITYBACKED_HPP
#define BLIB_GRAPHICS_COMPONENTS_ENTITYBACKED_HPP

#include <BLIB/ECS.hpp>

namespace bl
{
namespace gfx
{
namespace bcom
{
/**
 * @brief Base class for ECS-backed graphics objects
 *
 * @ingroup Graphics
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

protected:
    /**
     * @brief Creates the ECS entity for the drawable
     *
     * @param engine Game engine instance
     * @param flags The flags to create the entity with
     */
    void createEntityOnly(engine::Engine& engine, ecs::Flags flags = ecs::Flags::None);

    /**
     * @brief Returns the game engine instance. Only call after create()
     */
    engine::Engine& engine();

private:
    engine::Engine* enginePtr;
    ecs::Entity ecsId;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline ecs::Entity EntityBacked::entity() const { return ecsId; }

inline engine::Engine& EntityBacked::engine() { return *enginePtr; }

} // namespace bcom
} // namespace gfx
} // namespace bl

#endif
