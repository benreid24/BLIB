#ifndef BLIB_GRAPHICS_COMPONENTS_TRANSFORM2D_HPP
#define BLIB_GRAPHICS_COMPONENTS_TRANSFORM2D_HPP

#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/ECS.hpp>

namespace bl
{
namespace gfx
{
/// Base component classes for Graphics objects
namespace bcom
{
/**
 * @brief Base component class for Drawables that require a 2d transform
 *
 * @ingroup Graphics
 */
class Transform2D {
public:
    /**
     * @brief Does nothing
     */
    Transform2D() = default;

    /**
     * @brief Returns the transform. Must only be called after create()
     */
    com::Transform2D& getTransform();

    /**
     * @brief Returns the transform. Must only be called after create()
     */
    const com::Transform2D& getTransform() const;

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
    com::Transform2D* handle;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline com::Transform2D& Transform2D::getTransform() { return *handle; }

inline const com::Transform2D& Transform2D::getTransform() const { return *handle; }

template<typename... TArgs>
inline void Transform2D::create(ecs::Registry& registry, ecs::Entity entity, TArgs&&... args) {
    handle = registry.emplaceComponent<com::Transform2D>(entity, std::forward<TArgs>(args)...);
}

} // namespace bcom
} // namespace gfx
} // namespace bl

#endif
