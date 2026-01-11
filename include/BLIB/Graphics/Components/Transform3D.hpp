#ifndef BLIB_GRAPHICS_COMPONENTS_TRANSFORM3D_HPP
#define BLIB_GRAPHICS_COMPONENTS_TRANSFORM3D_HPP

#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/ECS.hpp>

namespace bl
{
namespace gfx
{
/// Base component classes for Graphics objects
namespace bcom
{
/**
 * @brief Base component class for Drawables that require a 3d transform
 *
 * @ingroup Graphics
 */
class Transform3D {
public:
    /**
     * @brief Does nothing
     */
    Transform3D() = default;

    /**
     * @brief Returns the transform. Must only be called after create()
     */
    com::Transform3D& getTransform();

    /**
     * @brief Returns the transform. Must only be called after create()
     */
    const com::Transform3D& getTransform() const;

protected:
    /**
     * @brief Creates the transform in the ECS
     *
     * @tparam ...TArgs Argument types to the transform constructor
     * @param registry The ECS registry instance
     * @param entity The entity id
     * @param ...args Arguments to the transform constructor
     */
    template<typename... TArgs>
    void create(ecs::Registry& registry, ecs::Entity entity, TArgs&&... args);

private:
    com::Transform3D* handle;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline com::Transform3D& Transform3D::getTransform() { return *handle; }

inline const com::Transform3D& Transform3D::getTransform() const { return *handle; }

template<typename... TArgs>
inline void Transform3D::create(ecs::Registry& registry, ecs::Entity entity, TArgs&&... args) {
    ecs::Transaction<ecs::tx::EntityRead,
                     ecs::tx::ComponentRead<>,
                     ecs::tx::ComponentWrite<com::Transform3D>>
        tx(registry);
    handle = registry.getComponent<com::Transform3D>(entity, tx);
    if (!handle) {
        handle = registry.emplaceComponentWithTx<com::Transform3D>(
            entity, tx, std::forward<TArgs>(args)...);
    }
}

} // namespace bcom
} // namespace gfx
} // namespace bl

#endif
