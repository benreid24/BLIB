#ifndef BLIB_RENDER_DRAWABLES_COMPONENTS_TRANSFORM2D_HPP
#define BLIB_RENDER_DRAWABLES_COMPONENTS_TRANSFORM2D_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Components/Transform2D.hpp>

namespace bl
{
namespace rc
{
namespace draw
{
/// Base component classes for Drawable objects
namespace base
{
/**
 * @brief Base component class for Drawables that require a 2d transform
 *
 * @ingroup Renderer
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
    ecs::StableHandle<com::Transform2D> handle;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline com::Transform2D& Transform2D::getTransform() { return handle.get(); }

inline const com::Transform2D& Transform2D::getTransform() const { return handle.get(); }

template<typename... TArgs>
inline void Transform2D::create(ecs::Registry& registry, ecs::Entity entity, TArgs&&... args) {
    registry.emplaceComponent<com::Transform2D>(entity, std::forward<TArgs>(args)...);
    handle.assign(registry, entity);
}

} // namespace base
} // namespace draw
} // namespace rc
} // namespace bl

#endif
